#include "texturedata.h"

#include "float16.h"

namespace sgd {

namespace {

template <class T, class S> T convert(S comp) {
	return comp;
}

// ***** To: uint8_t *****
template <> uint8_t convert<uint8_t>(int8_t comp) {
	return comp > 0 ? comp * 255 / 127 : 0;
}

template <> uint8_t convert<uint8_t>(uint16_t comp) {
	return comp * 255 / 65535;
}

template <> uint8_t convert<uint8_t>(int16_t comp) {
	return comp > 0 ? comp * 255 / 32767 : 0;
}

template <> uint8_t convert<uint8_t>(float16 comp) {
	return (uint8_t)std::round(std::clamp(float16ToFloat(comp), 0.0f, 1.0f) * 255.0f);
}

template <> uint8_t convert<uint8_t>(float comp) {
	return (uint8_t)std::round(std::clamp(comp, 0.0f, 1.0f) * 255.0f);
}

// ***** To: int8_t *****
template <> int8_t convert<int8_t>(uint8_t comp) {
	return (int8_t)(comp * 127 / 255);
}

template <> int8_t convert<int8_t>(uint16_t comp) {
	return (int8_t)(comp * 127 / 65535);
}

template <> int8_t convert<int8_t>(int16_t comp) {
	return (int8_t)(comp > 0 ? comp * 127 / 32767 : comp * -128 / -32768);
}

template <> int8_t convert<int8_t>(float16 comp) {
	return (int8_t)(std::round(std::clamp(float16ToFloat(comp), -1.0f, 1.0f) * 127.5f) - 0.5f);
}

template <> int8_t convert<int8_t>(float comp) {
	return (int8_t)(std::round(std::clamp(comp, -1.0f, 1.0f) * 127.5f) - 0.5f);
}

// ***** To: uint16_t *****
template <> uint16_t convert<uint16_t>(uint8_t comp) {
	return comp * 65535 / 255;
}

template <> uint16_t convert<uint16_t>(int8_t comp) {
	return comp > 0 ? comp * 65535 / 127 : 0;
}

template <> uint16_t convert<uint16_t>(int16_t comp) {
	return comp > 0 ? comp * 65535 / 32767 : 0;
}

template <> uint16_t convert<uint16_t>(float16 comp) {
	return (uint16_t)std::round(std::clamp(float16ToFloat(comp), 0.0f, 1.0f) * 65535.0f);
}

template <> uint16_t convert<uint16_t>(float comp) {
	return (uint16_t)std::round(std::clamp(comp, 0.0f, 1.0f) * 65535.0f);
}

// ***** To: int16_t *****
template <> int16_t convert<int16_t>(uint8_t comp) {
	return (int16_t)(comp > 0 ? comp * 32767 / 255 : 0);
}

template <> int16_t convert<int16_t>(int8_t comp) {
	return (int16_t)(comp > 0 ? comp * 32767 / 127 : comp * -32768 / -128);
}

template <> int16_t convert<int16_t>(uint16_t comp) {
	return (int16_t)(comp * 32767 / 65535);
}

template <> int16_t convert<int16_t>(float16 comp) {
	return (int16_t)(std::round(std::clamp(float16ToFloat(comp), -1.0f, 1.0f) * 32767.5f) - .5f);
}

template <> int16_t convert<int16_t>(float comp) {
	return (int16_t)(std::round(std::clamp(comp, -1.0f, 1.0f) * 32767.5f) - .5f);
}

// ***** To: float16 *****
template <> float16 convert<float16>(uint8_t comp) {
	return floatToFloat16(float(comp) / 255.0f);
}

template <> float16 convert<float16>(int8_t comp) {
	return floatToFloat16(((float)comp + 0.5f) / 127.5f);
}

template <> float16 convert<float16>(uint16_t comp) {
	return floatToFloat16(float(comp) / 65535.0f);
}

template <> float16 convert<float16>(int16_t comp) {
	return floatToFloat16(((float)comp + 0.5f) / 32767.5f);
}

template <> float16 convert<float16>(float comp) {
	return floatToFloat16(comp);
}

// ***** To: float *****
template <> float convert<float>(uint8_t comp) {
	return (float)comp / 255.0f;
}

template <> float convert<float>(int8_t comp) {
	return ((float)comp + 0.5f) / 127.5f;
}

template <> float convert<float>(uint16_t comp) {
	return float(comp) / 65535.0f;
}

template <> float convert<float>(int16_t comp) {
	return ((float)comp + 0.5f) / 32767.5f;
}

template <> float convert<float>(float16 comp) {
	return float16ToFloat(comp);
}

// ***** END conversions *****

template <TextureFormat> constexpr bool is_srgba8() {
	return false;
}

template <> constexpr bool is_srgba8<TextureFormat::srgba8>() {
	return true;
}

template <TextureFormat Fmt> struct TexelTraits {
	using CompType = void;
	static constexpr int numComps = 0;
};

#define TEXEL_TRAITS(FMT, T, N)                                                                                                \
	template <> struct TexelTraits<TextureFormat::FMT> {                                                                       \
		static constexpr TextureFormat format = TextureFormat::FMT;                                                            \
		using CompType = T;                                                                                                    \
		static constexpr int compCount = N;                                                                                    \
	};

TEXEL_TRAITS(r8, uint8_t, 1);
TEXEL_TRAITS(rg8, uint8_t, 2);
TEXEL_TRAITS(rgba8, uint8_t, 4);
TEXEL_TRAITS(srgba8, uint8_t, 4);
TEXEL_TRAITS(r8s, int8_t, 1);
TEXEL_TRAITS(rg8s, int8_t, 2);
TEXEL_TRAITS(rgba8s, int8_t, 4);
TEXEL_TRAITS(r16, uint16_t, 1);
TEXEL_TRAITS(rg16, uint16_t, 2);
TEXEL_TRAITS(rgba16, uint16_t, 4);
TEXEL_TRAITS(r16s, int16_t, 1);
TEXEL_TRAITS(rg16s, int16_t, 2);
TEXEL_TRAITS(rgba16s, int16_t, 4);
TEXEL_TRAITS(r16f, float16, 1);
TEXEL_TRAITS(rg16f, float16, 2);
TEXEL_TRAITS(rgba16f, float16, 4);
TEXEL_TRAITS(r32f, float, 1);
TEXEL_TRAITS(rg32f, float, 2);
TEXEL_TRAITS(rgba32f, float, 4);

template <TextureFormat SrcFormat, TextureFormat DstFormat>
void convert(const void* srcTexels, void* dstTexels, uint32_t count) {

	using SrcTraits = TexelTraits<SrcFormat>;
	using DstTraits = TexelTraits<DstFormat>;
	using SrcType = typename SrcTraits::CompType;
	using DstType = typename DstTraits::CompType;
	constexpr int srcCount = SrcTraits::compCount;
	constexpr int dstCount = DstTraits::compCount;

	if constexpr (is_srgba8<SrcFormat>() != is_srgba8<DstFormat>() && (is_srgba8<SrcFormat>() || is_srgba8<DstFormat>())) {
		SGD_ASSERT(srcCount == 4 && dstCount == 4);

		static Vector<float> tmp;
		if (count * srcCount > tmp.size()) tmp.resize(count * srcCount + 1023 & ~1023);

		constexpr float e = is_srgba8<DstFormat>() ? 1.0 / 2.2f : 2.2f;

		auto src = (SrcType*)srcTexels;
		auto dst = tmp.data();

		for (auto i = 0u; i < count; ++i) {
			dst[0] = std::pow(convert<float>(src[0]), e);
			dst[1] = std::pow(convert<float>(src[1]), e);
			dst[2] = std::pow(convert<float>(src[2]), e);
			dst[3] = convert<float>(src[3]);
			dst += srcCount;
			src += srcCount;
		}

		if constexpr (is_srgba8<DstFormat>()) {
			return convert<TextureFormat::rgba32f, TextureFormat::rgba8>(tmp.data(), dstTexels, count);
		} else {
			return convert<TextureFormat::rgba32f, DstFormat>(tmp.data(), dstTexels, count);
		}
	}

	static const auto zero = convert<DstType>(0.0f);
	static const auto one = convert<DstType>(1.0f);

	auto src = (SrcType*)srcTexels;
	auto dst = (DstType*)dstTexels;

	for (auto i = 0u; i < count; ++i) {
		dst[0] = convert<DstType>(src[0]);
		if constexpr (srcCount > 1) {
			if constexpr (dstCount > 1) dst[1] = convert<DstType>(src[1]);
		} else if constexpr (dstCount > 1) {
			dst[1] = zero;
		}
		if constexpr (srcCount > 2) {
			if constexpr (dstCount > 2) dst[2] = convert<DstType>(src[2]);
		} else if constexpr (dstCount > 2) {
			dst[2] = zero;
		}
		if constexpr (srcCount > 3) {
			if constexpr (dstCount > 3) dst[3] = convert<DstType>(src[3]);
		} else if constexpr (dstCount > 3) {
			dst[3] = one;
		}
		src += srcCount;
		dst += dstCount;
	}
}

template <TextureFormat SrcFormat> void convert(const void* src, void* dst, TextureFormat dstFormat, uint32_t count) {
	switch (dstFormat) {
	case TextureFormat::r8:
		return convert<SrcFormat, TextureFormat::r8>(src, dst, count);
	case TextureFormat::rg8:
		return convert<SrcFormat, TextureFormat::rg8>(src, dst, count);
	case TextureFormat::rgba8:
		return convert<SrcFormat, TextureFormat::rgba8>(src, dst, count);
	case TextureFormat::srgba8:
		return convert<SrcFormat, TextureFormat::srgba8>(src, dst, count);
	case TextureFormat::r8s:
		return convert<SrcFormat, TextureFormat::r8s>(src, dst, count);
	case TextureFormat::rg8s:
		return convert<SrcFormat, TextureFormat::rg8s>(src, dst, count);
	case TextureFormat::rgba8s:
		return convert<SrcFormat, TextureFormat::rgba8s>(src, dst, count);
	case TextureFormat::r16:
		return convert<SrcFormat, TextureFormat::r16>(src, dst, count);
	case TextureFormat::rg16:
		return convert<SrcFormat, TextureFormat::rg16>(src, dst, count);
	case TextureFormat::rgba16:
		return convert<SrcFormat, TextureFormat::rgba16>(src, dst, count);
	case TextureFormat::r16s:
		return convert<SrcFormat, TextureFormat::r16s>(src, dst, count);
	case TextureFormat::rg16s:
		return convert<SrcFormat, TextureFormat::rg16s>(src, dst, count);
	case TextureFormat::rgba16s:
		return convert<SrcFormat, TextureFormat::rgba16s>(src, dst, count);
	case TextureFormat::r16f:
		return convert<SrcFormat, TextureFormat::r16f>(src, dst, count);
	case TextureFormat::rg16f:
		return convert<SrcFormat, TextureFormat::rg16f>(src, dst, count);
	case TextureFormat::rgba16f:
		return convert<SrcFormat, TextureFormat::rgba16f>(src, dst, count);
	case TextureFormat::r32f:
		return convert<SrcFormat, TextureFormat::r32f>(src, dst, count);
	case TextureFormat::rg32f:
		return convert<SrcFormat, TextureFormat::rg32f>(src, dst, count);
	case TextureFormat::rgba32f:
		return convert<SrcFormat, TextureFormat::rgba32f>(src, dst, count);
	default:
		SGD_ABORT();
	}
}

} // namespace

void convertTexels(const void* src, TextureFormat srcFormat, void* dst, TextureFormat dstFormat, uint32_t count) {
	SGD_ASSERT(srcFormat != TextureFormat::any && dstFormat != TextureFormat::any);

	if (srcFormat == dstFormat) {
		std::memcpy(dst, src, bytesPerTexel(srcFormat) * count);
		return;
	}

	switch (srcFormat) {
	case TextureFormat::r8:
		return convert<TextureFormat::r8>(src, dst, dstFormat, count);
	case TextureFormat::rg8:
		return convert<TextureFormat::rg8>(src, dst, dstFormat, count);
	case TextureFormat::rgba8:
		return convert<TextureFormat::rgba8>(src, dst, dstFormat, count);
	case TextureFormat::srgba8:
		return convert<TextureFormat::srgba8>(src, dst, dstFormat, count);
	case TextureFormat::r8s:
		return convert<TextureFormat::r8s>(src, dst, dstFormat, count);
	case TextureFormat::rg8s:
		return convert<TextureFormat::rg8s>(src, dst, dstFormat, count);
	case TextureFormat::rgba8s:
		return convert<TextureFormat::rgba8s>(src, dst, dstFormat, count);
	case TextureFormat::r16:
		return convert<TextureFormat::r16>(src, dst, dstFormat, count);
	case TextureFormat::rg16:
		return convert<TextureFormat::rg16>(src, dst, dstFormat, count);
	case TextureFormat::rgba16:
		return convert<TextureFormat::rgba16>(src, dst, dstFormat, count);
	case TextureFormat::r16s:
		return convert<TextureFormat::r16s>(src, dst, dstFormat, count);
	case TextureFormat::rg16s:
		return convert<TextureFormat::rg16s>(src, dst, dstFormat, count);
	case TextureFormat::rgba16s:
		return convert<TextureFormat::rgba16s>(src, dst, dstFormat, count);
	case TextureFormat::r16f:
		return convert<TextureFormat::r16f>(src, dst, dstFormat, count);
	case TextureFormat::rg16f:
		return convert<TextureFormat::rg16f>(src, dst, dstFormat, count);
	case TextureFormat::rgba16f:
		return convert<TextureFormat::rgba16f>(src, dst, dstFormat, count);
	case TextureFormat::r32f:
		return convert<TextureFormat::r32f>(src, dst, dstFormat, count);
	case TextureFormat::rg32f:
		return convert<TextureFormat::rg32f>(src, dst, dstFormat, count);
	case TextureFormat::rgba32f:
		return convert<TextureFormat::rgba32f>(src, dst, dstFormat, count);
	default:
		SGD_ABORT();
	}
}

TextureData* convert(CTextureData* src, TextureFormat dstFormat) {
	auto dst = new TextureData(src->size(), dstFormat);

	for (auto y = 0u; y < src->size().y; ++y) {
		convertTexels(texelPtr(src, {0, y}), src->format(), texelPtr(dst, {0, y}), dstFormat, src->size().x);
	}

	return dst;
}

TextureData* extractCubemap(CTextureData* data) {
	//	if (data->size().y == data->size().x * 6) return data;

	const Vec2u* orgs;
	uint32_t faceSize;

	if (data->size().x * 3 == data->size().y * 4) {
		//    +y
		// -x +z +x -z
		//    -y
		static Vec2u _orgs[]{
			//	static Array<Vec2u, 6> orgs{	// Can't init std::array like this?
			{2, 1}, //+x
			{0, 1}, //-x
			{1, 0}, //+y
			{1, 2}, //-y
			{1, 1}, //+z
			{3, 1}, //-z
		};
		orgs = _orgs;
		faceSize = data->size().x / 4;
	} else {
		return nullptr;
	}

	auto dstPitch = faceSize * data->bpp();
	auto srcPitch = data->pitch();
	auto out = new TextureData({faceSize, faceSize * 6}, data->format());

	auto dstp = out->data();
	for (int i = 0; i < 6; ++i) {
		auto srcp = data->data() + orgs[i].y * faceSize * srcPitch + orgs[i].x * dstPitch;
		for (int y = 0; y < faceSize; ++y) {
			std::memcpy(dstp, srcp, dstPitch);
			srcp += srcPitch;
			dstp += dstPitch;
		}
	}
	return out;
}

namespace {

template <class T> void premultiplyAlphaSRGBA(TextureData* data) {
	for (auto y = 0u; y < data->size().y; ++y) {
		auto p = (T*)texelPtr(data, {0, y});
		for (auto x = 0u; x < data->size().x; ++x) {
			auto alpha = convert<float>(p[3]);
			p[0] = convert<T>(std::pow(std::pow(convert<float>(p[0]), 2.2f) * alpha, 1.0f / 2.2f));
			p[1] = convert<T>(std::pow(std::pow(convert<float>(p[1]), 2.2f) * alpha, 1.0f / 2.2f));
			p[2] = convert<T>(std::pow(std::pow(convert<float>(p[2]), 2.2f) * alpha, 1.0f / 2.2f));
			p += 4;
		}
	}
}

template <class T> void premultiplyAlpha(TextureData* data) {
	for (auto y = 0u; y < data->size().y; ++y) {
		auto p = (T*)texelPtr(data, {0, y});
		for (auto x = 0u; x < data->size().x; ++x) {
			auto alpha = convert<float>(p[3]);
			p[0] = convert<T>(convert<float>(p[0]) * alpha);
			p[1] = convert<T>(convert<float>(p[1]) * alpha);
			p[2] = convert<T>(convert<float>(p[2]) * alpha);
			p += 4;
		}
	}
}

} // namespace

void premultiplyAlpha(TextureData* data) {
	switch (data->format()) {
	case TextureFormat::rgba8:
		return premultiplyAlpha<uint8_t>(data);
	case TextureFormat::srgba8:
		return premultiplyAlphaSRGBA<uint8_t>(data);
	case TextureFormat::rgba8s:
		return premultiplyAlpha<int8_t>(data);
	case TextureFormat::rgba16:
		return premultiplyAlpha<uint16_t>(data);
	case TextureFormat::rgba16s:
		return premultiplyAlpha<int16_t>(data);
	case TextureFormat::rgba16f:
		return premultiplyAlpha<float16>(data);
	case TextureFormat::rgba32f:
		return premultiplyAlpha<float>(data);
	default:
		SGD_ABORT();
	}
}

} // namespace sgd
