#include "textureutil.h"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace sgd {

const Texture* rgbaTexture(uint32_t rgba) {

	static Map<uint32_t, TexturePtr> cache;

	auto& texture = cache[rgba];
	if (texture) return texture;

	texture = new Texture({1, 1}, 1, TextureFormat::rgba8, TextureFlags::none);

	texture->update(&rgba, bytesPerTexel(TextureFormat::rgba8));

	return texture;
}

Expected<Texture*, FileioEx> loadTexture(CData data, TextureFormat format, TextureFlags flags, uint32_t depth) {

	if (format != TextureFormat::rgba8 && format != TextureFormat::srgba8) {
		return FileioEx("Invalid texture format");
	}

	if (bool(flags & TextureFlags::array) && bool(flags & TextureFlags::cube)) {
		return FileioEx("Invalid combination of texture flags");
	}

	if (bool(flags & TextureFlags::array)) {
		if (!depth) return FileioEx("Texture array depth must be greater than 0");
	} else if (bool(flags & TextureFlags::cube)) {
		if (!depth) depth = 6;
		if (depth != 6) return FileioEx("Cube texture depth must be equal to 6");
	} else {
		if (!depth) depth = 1;
		if (depth != 1) return FileioEx("Texture Depth must be 1");
	}

	auto bpp = bytesPerTexel(format);

	Vec2u size;
	uint32_t n;
	auto img = stbi_load_from_memory(data.data(), data.size(), (int*)&size.x, (int*)&size.y, (int*)&n, (int)bpp);
	auto free = &stbi_image_free;

	if (!img) return FileioEx("STB error decoding image data");

	if (bool(flags & TextureFlags::array)) {

		if (size.y % depth != 0) {
			free(img);
			return FileioEx("Texture array image height must be a multiple of depth");
		}

		size.y /= depth;

	} else if (bool(flags & TextureFlags::cube) && size.y != size.x * 6) {

		const Vec2u* orgs;
		uint32_t sz;

		if (size.x * 3 == size.y * 4) {
			//    +y
			// -x +z +x -z
			//    -y
			static constexpr Vec2u _orgs[]{
				//	static Array<Vec2u, 6> orgs{	// Can't init std::array like this?
				{2, 1}, //+x
				{0, 1}, //-x
				{1, 0}, //+y
				{1, 2}, //-y
				{1, 1}, //+z
				{3, 1}, //-z
			};
			orgs = _orgs;
			sz = size.x / 4;
		} else {
			free(img);
			return FileioEx("Invalid cube texture image layout");
		}

		auto srcPitch = size.x * bpp;
		auto dstPitch = sz * bpp;
		auto dst = (uint8_t*)std::malloc(dstPitch * sz * 6);

		auto dstp = dst;
		for (int i = 0; i < 6; ++i) {
			auto srcp = img + orgs[i].y * sz * srcPitch + orgs[i].x * sz * bpp;

			for (int y = 0; y < sz; ++y) {
				std::memcpy(dstp, srcp, dstPitch);
				srcp += srcPitch;
				dstp += dstPitch;
			}
		}
		free(img);
		img = dst;
		free = &std::free;
		size = {sz, sz};
	}

	premultiplyAlpha(img, format, size, size.x * bpp);

	auto texture = new Texture(size, depth, format, flags);

	texture->update(img, size.x * bpp);

	free(img);

	return texture;
}

Expected<Texture*, FileioEx> loadTexture(CPath path, TextureFormat format, TextureFlags flags, uint32_t depth) {

	auto data = loadData(path);
	if (!data) return data.error();

	return loadTexture(data.result(), format, flags, depth);
}

void premultiplyAlpha(void* data, TextureFormat format, CVec2u size, uint32_t pitch) {
	if (format != TextureFormat::rgba8 && format != TextureFormat::srgba8) return;

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

} // namespace sgd
