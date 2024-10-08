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

namespace {

int loadEXRFromMemory(float** out_rgba, int* width, int* height, int* channels, //
					  const unsigned char* memory, size_t size, const char** err) {
	if (out_rgba == NULL || memory == NULL) {
		tinyexr::SetErrorMessage("Invalid argument for LoadEXRFromMemory", err);
		return TINYEXR_ERROR_INVALID_ARGUMENT;
	}

	EXRVersion exr_version;
	EXRImage exr_image;
	EXRHeader exr_header;

	InitEXRHeader(&exr_header);

	int ret = ParseEXRVersionFromMemory(&exr_version, memory, size);
	if (ret != TINYEXR_SUCCESS) {
		std::stringstream ss;
		ss << "Failed to parse EXR version. code(" << ret << ")";
		tinyexr::SetErrorMessage(ss.str(), err);
		return ret;
	}

	ret = ParseEXRHeaderFromMemory(&exr_header, &exr_version, memory, size, err);
	if (ret != TINYEXR_SUCCESS) {
		return ret;
	}

	// Read HALF channel as FLOAT.
	for (int i = 0; i < exr_header.num_channels; i++) {
		if (exr_header.pixel_types[i] == TINYEXR_PIXELTYPE_HALF) {
			exr_header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
		}
	}

	InitEXRImage(&exr_image);
	ret = LoadEXRImageFromMemory(&exr_image, &exr_header, memory, size, err);
	if (ret != TINYEXR_SUCCESS) {
		return ret;
	}

	// RGBA
	int idxR = -1;
	int idxG = -1;
	int idxB = -1;
	int idxA = -1;
	for (int c = 0; c < exr_header.num_channels; c++) {
		if (strcmp(exr_header.channels[c].name, "R") == 0) {
			idxR = c;
		} else if (strcmp(exr_header.channels[c].name, "G") == 0) {
			idxG = c;
		} else if (strcmp(exr_header.channels[c].name, "B") == 0) {
			idxB = c;
		} else if (strcmp(exr_header.channels[c].name, "A") == 0) {
			idxA = c;
		}
	}

	// TODO(syoyo): Refactor removing same code as used in LoadEXR().
	if (exr_header.num_channels == 1) {
		// Grayscale channel only.

		(*out_rgba) = reinterpret_cast<float*>(
			malloc(sizeof(float) * static_cast<size_t>(exr_image.width) * static_cast<size_t>(exr_image.height)));

		if (exr_header.tiled) {
			const size_t tile_size_x = static_cast<size_t>(exr_header.tile_size_x);
			const size_t tile_size_y = static_cast<size_t>(exr_header.tile_size_y);
			for (int it = 0; it < exr_image.num_tiles; it++) {
				for (size_t j = 0; j < tile_size_y; j++) {
					for (size_t i = 0; i < tile_size_x; i++) {
						const size_t ii = static_cast<size_t>(exr_image.tiles[it].offset_x) * tile_size_x + i;
						const size_t jj = static_cast<size_t>(exr_image.tiles[it].offset_y) * tile_size_y + j;
						const size_t idx = ii + jj * static_cast<size_t>(exr_image.width);

						// out of region check.
						if (ii >= static_cast<size_t>(exr_image.width)) {
							continue;
						}
						if (jj >= static_cast<size_t>(exr_image.height)) {
							continue;
						}
						const size_t srcIdx = i + j * tile_size_x;
						unsigned char** src = exr_image.tiles[it].images;

						(*out_rgba)[idx] = reinterpret_cast<float**>(src)[0][srcIdx];
					}
				}
			}
		} else {
			const size_t pixel_size = static_cast<size_t>(exr_image.width) * static_cast<size_t>(exr_image.height);
			for (size_t i = 0; i < pixel_size; i++) {
				const float val = reinterpret_cast<float**>(exr_image.images)[0][i];
				(*out_rgba)[i] = val;
			}
		}

	} else {
		// TODO(syoyo): Support non RGBA image.

		if (idxR == -1) {
			tinyexr::SetErrorMessage("R channel not found", err);

			// @todo { free exr_image }
			return TINYEXR_ERROR_INVALID_DATA;
		}

		if (idxG == -1) {
			tinyexr::SetErrorMessage("G channel not found", err);
			// @todo { free exr_image }
			return TINYEXR_ERROR_INVALID_DATA;
		}

		if (idxB == -1) {
			tinyexr::SetErrorMessage("B channel not found", err);
			// @todo { free exr_image }
			return TINYEXR_ERROR_INVALID_DATA;
		}

		(*out_rgba) = reinterpret_cast<float*>(
			malloc(4 * sizeof(float) * static_cast<size_t>(exr_image.width) * static_cast<size_t>(exr_image.height)));

		if (exr_header.tiled) {
			const size_t tile_size_x = static_cast<size_t>(exr_header.tile_size_x);
			const size_t tile_size_y = static_cast<size_t>(exr_header.tile_size_y);
			for (int it = 0; it < exr_image.num_tiles; it++) {
				for (size_t j = 0; j < tile_size_y; j++)
					for (size_t i = 0; i < tile_size_x; i++) {
						const size_t ii = static_cast<size_t>(exr_image.tiles[it].offset_x) * tile_size_x + i;
						const size_t jj = static_cast<size_t>(exr_image.tiles[it].offset_y) * tile_size_y + j;
						const size_t idx = ii + jj * static_cast<size_t>(exr_image.width);

						// out of region check.
						if (ii >= static_cast<size_t>(exr_image.width)) {
							continue;
						}
						if (jj >= static_cast<size_t>(exr_image.height)) {
							continue;
						}
						const size_t srcIdx = i + j * tile_size_x;
						unsigned char** src = exr_image.tiles[it].images;
						(*out_rgba)[4 * idx + 0] = reinterpret_cast<float**>(src)[idxR][srcIdx];
						(*out_rgba)[4 * idx + 1] = reinterpret_cast<float**>(src)[idxG][srcIdx];
						(*out_rgba)[4 * idx + 2] = reinterpret_cast<float**>(src)[idxB][srcIdx];
						if (idxA != -1) {
							(*out_rgba)[4 * idx + 3] = reinterpret_cast<float**>(src)[idxA][srcIdx];
						} else {
							(*out_rgba)[4 * idx + 3] = 1.0;
						}
					}
			}
		} else {
			const size_t pixel_size = static_cast<size_t>(exr_image.width) * static_cast<size_t>(exr_image.height);
			for (size_t i = 0; i < pixel_size; i++) {
				(*out_rgba)[4 * i + 0] = reinterpret_cast<float**>(exr_image.images)[idxR][i];
				(*out_rgba)[4 * i + 1] = reinterpret_cast<float**>(exr_image.images)[idxG][i];
				(*out_rgba)[4 * i + 2] = reinterpret_cast<float**>(exr_image.images)[idxB][i];
				if (idxA != -1) {
					(*out_rgba)[4 * i + 3] = reinterpret_cast<float**>(exr_image.images)[idxA][i];
				} else {
					(*out_rgba)[4 * i + 3] = 1.0;
				}
			}
		}
	}

	(*width) = exr_image.width;
	(*height) = exr_image.height;
	(*channels) = exr_image.num_channels;

	FreeEXRHeader(&exr_header);
	FreeEXRImage(&exr_image);

	return TINYEXR_SUCCESS;
}

TextureFormat floatFormat(int channels) {
	if (sgd::contains(wgpuRequiredFeatures(), wgpu::FeatureName::Float32Filterable)) {
		TextureFormat formats[]{{}, TextureFormat::r32f, TextureFormat::rg32f, TextureFormat::rgba32f, TextureFormat::rgba32f};
		return formats[channels];
	} else {
		TextureFormat formats[]{{}, TextureFormat::r16f, TextureFormat::rg16f, TextureFormat::rgba16f, TextureFormat::rgba16f};
		return formats[channels];
	}
}

} // namespace

Expected<TextureData*, FileioEx> loadTextureData(CData fileData, TextureFormat format) {

	TextureDataPtr data;

	const char exrHeader[] = {0x76, 0x2f, 0x31, 0x01};
	if (fileData.size() >= 4 && !std::memcmp(fileData.data(), exrHeader, 4)) {
		//
		SGD_LOG << "### Loading EXR";
		//
		// Use tinyexr
		//
		float* img{};
		Vec2i size{};
		int channels{};
		const char* err{};
		if (loadEXRFromMemory(&img, &size.x, &size.y, &channels, fileData.data(), fileData.size(), &err)) {
			return SGD_FILEIOEX(String("TinyEXR error decoding image data: ") + (err ? err : "unknown error"));
		}
		//
		//SGD_ASSERT(channels == 1 || channels == 4);
		//
		auto srcFormat = (channels > 1) ? TextureFormat::rgba32f : TextureFormat::r32f;
		if (format == TextureFormat::any) format = floatFormat(channels);
		//
		data = new TextureData(size, srcFormat, img);
		data->deleted.connect(nullptr, [=] { free(img); });
		//
	} else if (stbi_is_hdr_from_memory(fileData.data(), (int)fileData.size())) {
		//
		// Use STB HDR
		//
		SGD_LOG << "### Loading STB HDR";
		//
		Vec2i size{};
		int channels{};
		auto img = stbi_loadf_from_memory(fileData.data(), (int)fileData.size(), &size.x, &size.y, &channels, 4);
		if (!img) return SGD_FILEIOEX("STB image error decoding hdr image data");
		//
		auto srcFormat = TextureFormat::rgba32f;
		if (format == TextureFormat::any) floatFormat(channels);
		//
		data = new TextureData(size, srcFormat, img);
		data->deleted.connect(nullptr, [=] { stbi_image_free(img); });
		//
	} else if (stbi_is_16_bit_from_memory(fileData.data(), (int)fileData.size())) {
		//
		// Use STB 16 bit
		//
		SGD_LOG << "### Loading STB 16";
		//
		Vec2i size{};
		int channels{};
		auto img = stbi_load_16_from_memory(fileData.data(), (int)fileData.size(), &size.x, &size.y, &channels, 4);
		if (!img) return SGD_FILEIOEX("STB image error decoding 16 bit image data");
		//
		auto srcFormat = TextureFormat::srgba16;
		if (format == TextureFormat::any) format = floatFormat(channels);
		//
		data = new TextureData(size, srcFormat, img);
		data->deleted.connect(nullptr, [=] { stbi_image_free(img); });
		//
	} else {
		//
		// Use STB 8
		//
		SGD_LOG << "### Loading STB 8";
		//
		Vec2i size{};
		int channels{};
		auto img = stbi_load_from_memory(fileData.data(), (int)fileData.size(), &size.x, &size.y, &channels, 4);
		if (!img) return SGD_FILEIOEX("STB image errror decoding 8 bit image data");
		//
		if (format == TextureFormat::any) format = TextureFormat::srgba8;
		auto srcFormat = (format == TextureFormat::srgba8) ? format : TextureFormat::rgba8;
		//
		data = new TextureData(size, srcFormat, img);
		data->deleted.connect(nullptr, [=] { stbi_image_free(img); });
		//
	}

	if (format != data->format()) {
		data = convert(data, format);
	}

	if (format == TextureFormat::srgba8 || format == TextureFormat::srgba16) {
		SGD_ASSERT(format != TextureFormat::srgba16);
		premultiplyAlpha(data);
	}
	return data.reset();
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

Expected<Texture*, FileioEx> loadArrayTexture(CPath path, TextureFormat format, TextureFlags flags, uint32_t frameCount,
											  uint32_t framesX, uint32_t framesY, uint32_t frameSpacing) {
	if (!frameCount || !framesX || !framesY) SGD_ERROR("Invalid frame layout parameters");

	auto tdata = loadTextureData(path, format);
	if (!tdata) return tdata.error();

	TextureDataPtr srcData = tdata.result();
	auto size = srcData->size();
	auto frameSize = Vec2u((size.x - (framesX - 1) * frameSpacing) / framesX, //
						   (size.y - (framesY - 1) * frameSpacing) / framesY);

	if (!frameSize.x || !frameSize.y) return SGD_PATHEX("Invalid frame layout parameters", path);

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
