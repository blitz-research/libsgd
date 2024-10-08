#include "textureutil.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define TINYEXR_USE_MINIZ 0
#define TINYEXR_USE_STB_ZLIB 1
#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>

#include "float16.h"

namespace sgd {

Expected<TextureData*, FileioEx> loadTextureData(CData fileData, TextureFormat format) {

	TextureDataPtr dataPtr;

	const char exrHeader[] = {0x76, 0x2f, 0x31, 0x01};
	if (fileData.size() >= 4 && !std::memcmp(fileData.data(), exrHeader, 4)) {
		//
		// Use tinyexr
		//
		float* img{};
		Vec2i size{};
		const char* err{};
		if (LoadEXRFromMemory(&img, &size.x, &size.y, fileData.data(), fileData.size(), &err)) {
			return SGD_FILEIOEX(String("Failed to decode image file data: ") + (err ? err : "unknown error"));
		}
		if (format == TextureFormat::any) format = TextureFormat::rgba16f;
		//
		dataPtr = new TextureData(size, TextureFormat::rgba32f, img);
		dataPtr->deleted.connect(nullptr, [=] { free(img); });
		//
	} else if (stbi_is_hdr_from_memory(fileData.data(), (int)fileData.size())) {
		//
		// Use STB float
		//
		Vec2i size{};
		auto img = stbi_loadf_from_memory(fileData.data(), (int)fileData.size(), &size.x, &size.y, nullptr, 4);
		if (!img) return SGD_FILEIOEX(String("Error decoding image file data"));
		//
		if (format == TextureFormat::any) format = TextureFormat::rgba16f;
		//
		dataPtr = new TextureData(size, TextureFormat::rgba32f, img);
		dataPtr->deleted.connect(nullptr, [=] { stbi_image_free(img); });
		//
	} else {
		//
		// Use STB
		//
		Vec2i size{};
		auto img = stbi_load_from_memory(fileData.data(), (int)fileData.size(), &size.x, &size.y, nullptr, 4);
		if (!img) {
			return SGD_FILEIOEX(String("Error decoding image file data"));
		}
		//
		if (format == TextureFormat::any) format = TextureFormat::srgba8;
		auto srcFormat = (format == TextureFormat::srgba8) ? format : TextureFormat::rgba8;
		//
		dataPtr = new TextureData(size, srcFormat, img);
		dataPtr->deleted.connect(nullptr, [=] { stbi_image_free(img); });
		//
	}
	if (format != dataPtr->format()) {
		dataPtr = convert(dataPtr, format);
	}
	if (format == TextureFormat::srgba8) {
		premultiplyAlpha(dataPtr);
	}
	return dataPtr.reset();
}

Expected<TextureData*, FileioEx> loadTextureData(CPath path, TextureFormat format) {
	auto data = loadData(path);
	if (!data) return data.error();

	auto texture = loadTextureData(data.result(), format);
	if (!texture) return SGD_PATHEX("Error loading texture", path);

	return texture.result();
}

Expected<Texture*, FileioEx> load2DTexture(CPath path, TextureFormat format, TextureFlags flags) {
	auto texData = loadTextureData(path, format);
	if (!texData) return texData.error();

	TextureDataPtr data = texData.result();
	return new Texture({data->size().x, data->size().y}, 1, data->format(), flags, data);
}

Expected<Texture*, FileioEx> loadCubeTexture(CPath path, TextureFormat format, TextureFlags flags) {
	auto texData = loadTextureData(path, format);
	if (!texData) return texData.error();

	TextureDataPtr data = texData.result();
	if (data->size().y != data->size().x * 6) {
		data = extractCubemap(data);
		if (!data) return SGD_PATHEX("Invalid cubemap texture layout", path);
	}
	return new Texture({data->size().x, data->size().y / 6}, 6, data->format(), flags | TextureFlags::cube, data);
}

Expected<Texture*, FileioEx> loadArrayTexture(CPath path, TextureFormat format, TextureFlags flags) {
	auto texData = loadTextureData(path, format);
	if (!texData) return texData.error();

	TextureDataPtr data = texData.result();
	uint32_t depth = 1;

	return new Texture({data->size().x, data->size().y / depth}, depth, data->format(), flags | TextureFlags::array, data);
}

Expected<Texture*, FileioEx> loadArrayTexture(CPath path, TextureFormat format, TextureFlags flags, uint32_t frameCount, uint32_t framesX, uint32_t framesY,
											  uint32_t frameSpacing) {
	if (!frameCount || !framesX || !framesY) SGD_ERROR("Invalid frame layout parameters");
	
	auto tdata = loadTextureData(path, format);
	if (!tdata) return tdata.error();
	
	TextureDataPtr srcData = tdata.result();
	auto size = srcData->size();
	auto frameSize = Vec2u((size.x - (framesX - 1) * frameSpacing) / framesX, //
						   (size.y - (framesY - 1) * frameSpacing) / framesY);

	if(!frameSize.x || !frameSize.y) return SGD_PATHEX("Invalid frame layout parameters", path);

	Vector<Vec2u> orgs;
	Vec2u pos;
	for (auto i = 0; i < frameCount; ++i) {
		orgs.push_back(pos);
		pos.x += frameSize.x + frameSpacing;
		if (pos.x + frameSize.x > size.x) {
			pos.x = 0;
			pos.y += frameSize.y + frameSpacing;
		}
	}

	auto texture = new Texture(frameSize, frameCount, srcData->format(), flags);

	auto dstData = texture->lock();

	auto dst = dstData->data();
	auto bpr = size.x * dstData->bpp();

	for (auto i = 0; i < frameCount; ++i) {
		auto src = srcData->data() + orgs[i].y * srcData->pitch() + orgs[i].x * srcData->bpp();
		for (auto y = 0u; y < frameSize.y; ++y) {
			std::memcpy(dst, src, bpr);
			dst += dstData->pitch();
			src += srcData->pitch();
		}
	}
	texture->unlock();

	return texture;
}

CTexture* rgbaTexture(uint32_t rgba, TextureFlags flags) {

	static Map<std::pair<uint32_t, TextureFlags>, TexturePtr> cache;

	auto& texture = cache[std::make_pair(rgba, flags)];
	if (texture) return texture;

	uint32_t depth = bool(flags & TextureFlags::cube) ? 6 : 1;
	texture = new Texture({1, 1}, depth, TextureFormat::rgba8, flags);
	uint32_t data[]{rgba, rgba, rgba, rgba, rgba, rgba};
	texture->update(data, sizeof(rgba));

	return texture;
}

CTexture* dummyTexture(TextureFormat format, TextureFlags flags) {

	static Map<std::pair<TextureFormat, TextureFlags>, TexturePtr> cache;

	auto& texture = cache[std::make_pair(format, flags)];
	if (texture) return texture;

	uint32_t depth = bool(flags & TextureFlags::cube) ? 6 : 1;
	return texture = new Texture({1, 1}, depth, format, flags);
}

} // namespace sgd
