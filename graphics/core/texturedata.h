#pragma once

#include <geom/exports.h>

namespace sgd {

//! TextureFormat enum
enum struct TextureFormat {
	any,
	// 8 bit unsigned normalized formats
	r8,
	rg8,
	rgba8,
	srgba8,
	// 8 bit signed normalized formats (note: no srgba8s in dawn)
	r8s,
	rg8s,
	rgba8s,
	// 16 bit unsigned normalized formats
	r16,
	rg16,
	rgba16,
	// 16 bit signed normalized formats
	r16s,
	rg16s,
	rgba16s,
	// 16 bit float formats
	r16f,
	rg16f,
	rgba16f,
	// 32 bit float formats
	r32f,
	rg32f,
	rgba32f,
	// depth formats
	depth32f,
};
static constexpr int numTextureFormats = 21;

inline size_t bytesPerTexel(TextureFormat format) {
	uint32_t bpp[]{0,			 // undefined
				   1, 2, 4,	 4,	 // 8 bit unsigned
				   1, 2, 4,		 // 8 bit signed
				   2, 4, 8,		 // 16 bit unsigned
				   2, 4, 8,		 // 16 bit signed
				   2, 4, 8,		 // 16f
				   4, 8, 16,	 // 32f
				   4};			 // depth32f
	if ((uint32_t)format >= numTextureFormats) SGD_ERROR("Invalid texel format");
	return bpp[(uint32_t)format];
}

SGD_SHARED(TextureData);

// Move to own file?
struct TextureData : Shared {
	SGD_OBJECT_TYPE(TextureData, Shared);

	TextureData(CVec2u size, TextureFormat format, void* data)
		: m_size(size), m_format(format), m_bpp(bytesPerTexel(format)), m_pitch(size.x * m_bpp), m_data((uint8_t*)data) {
	}

	TextureData(CVec2u size, TextureFormat format)
		: m_size(size), m_format(format), m_bpp(bytesPerTexel(format)), m_pitch(size.x * m_bpp),
		  m_data((uint8_t*)std::malloc(m_pitch * size.y)) {
		this->deleted.connect(nullptr, [=] { std::free(m_data); });
	}

	CVec2u size() const {
		return m_size;
	}

	TextureFormat format() const {
		return m_format;
	}

	size_t bpp() const {
		return m_bpp;
	}

	size_t pitch() const {
		return m_pitch;
	}

	const uint8_t* data() const {
		return m_data;
	}

	uint8_t* data() {
		return m_data;
	}

private:
	Vec2u m_size;
	TextureFormat m_format;
	size_t m_bpp;
	size_t m_pitch;
	uint8_t* m_data;
};

inline uint8_t* texelPtr(TextureData* data, CVec2u pos) {
	return data->data() + pos.y * data->pitch() + pos.x * data->bpp();
}

inline const uint8_t* texelPtr(CTextureData* data, CVec2u pos) {
	return data->data() + pos.y * data->pitch() + pos.x * data->bpp();
}

void convertTexels(const void* srcTexels, TextureFormat srcFormat, void* dstTexels, TextureFormat dstFormat, uint32_t count);

TextureData* convert(CTextureData* data, TextureFormat format);

TextureData* resize(CTextureData* data, CVec2u size);

TextureData* extractCubemap(CTextureData* data);

void premultiplyAlpha(TextureData* data);

} // namespace sgd
