#include "textureutil.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace sgd {

void premultiplyAlpha(void* data, TextureFormat format, CVec2u size, uint32_t pitch) {

	auto bpp = bytesPerTexel(format);

	switch (format) {
	case TextureFormat::rgba8:
		for (auto y = 0; y < size.y; ++y) {
			auto p = (uint8_t*)data + y * pitch;
			for (auto x = 0; x < size.x; p += 4, ++x) {
				auto a = u8tof(p[3]);
				p[0] = ftou8(std::pow(std::pow(u8tof(p[0]), 2.2f) * a, 1.0f / 2.2f));
				p[1] = ftou8(std::pow(std::pow(u8tof(p[1]), 2.2f) * a, 1.0f / 2.2f));
				p[2] = ftou8(std::pow(std::pow(u8tof(p[2]), 2.2f) * a, 1.0f / 2.2f));
			}
		}
		break;
	case TextureFormat::srgba8:
		for (auto y = 0; y < size.y; ++y) {
			auto p = (uint8_t*)data + y * pitch;
			for (auto x = 0; x < size.x; p += 4, ++x) {
				p[0] = p[0] * p[3] / 255;
				p[1] = p[1] * p[3] / 255;
				p[2] = p[2] * p[3] / 255;
			}
		}
		break;
	default:
		break;
	}
}

Expected<Texture*, FileioEx> loadTexture(CData data, TextureFormat format, TextureFlags flags) {

	if (format != TextureFormat::rgba8 && format != TextureFormat::srgba8) SGD_ABORT();

	int bpp = bytesPerTexel(format);

	Vec2u size;
	uint32_t n;

	auto img = stbi_load_from_memory(data.data(), data.size(), (int*)&size.x, (int*)&size.y, (int*)&n, bpp);
	if (!img) return FileioEx("STB error decoding image data");

	uint32_t depth = 1;

	if (bool(flags & TextureFlags::cube) && size.y != size.x * 6) {

		// 4 x 3 cell layout?
		if (size.x * 3 == size.y * 4) {

			auto sqsize = size.x / 4;

			auto srcPitch = size.x * bpp;
			auto dstPitch = sqsize * bpp;
			auto dstImg = (uint8_t*)malloc(dstPitch * sqsize * 6);

			for (int i = 0; i < 6; ++i) {
				static constexpr Vec2u orgs[]{
					//	static Array<Vec2u, 6> orgs{	// Can't init std::array like this?
					{2, 1}, //+x
					{0, 1}, //-x
					{1, 0}, //+y
					{1, 2}, //-y
					{1, 1}, //+z
					{3, 1}, //-z
				};
				auto src = img + orgs[i].y * sqsize * srcPitch + orgs[i].x * sqsize * bpp;
				auto dst = dstImg + i * sqsize * dstPitch;
				for (int y = 0; y < sqsize; ++y) {
					std::memcpy(dst, src, sqsize * bpp);
					src += srcPitch;
					dst += dstPitch;
				}
			}
			stbi_image_free(img);
			img = dstImg;
			size = {sqsize, sqsize};
			depth = 6;
		} else {
			SGD_ABORT();
		}
	}

	premultiplyAlpha(img, format, size, size.x * bpp);

	auto texture = new Texture(size, depth, format, flags);

	texture->update(img, size.x * bpp);

	stbi_image_free(img);

	return texture;
}

void premultiplyAlpha(Texture* texture) {

}


Expected<Texture*, FileioEx> loadTexture(CPath path, TextureFormat format, TextureFlags flags) {

	auto data = loadData(path);
	if (!data) return data.error();

	return loadTexture(data.result(), format, flags);
}

Texture* rgbaTexture(uint32_t rgba) {

	static Map<uint32_t, TexturePtr> cache;

	auto& texture = cache[rgba];
	if (texture) return texture;

	texture = new Texture({1, 1}, 1, TextureFormat::rgba8, TextureFlags::none);

	texture->update(&rgba, bytesPerTexel(TextureFormat::rgba8));

	return texture;
}

} // namespace sgd
