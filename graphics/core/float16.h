#pragma once

#include <geom/exports.h>

namespace sgd {

// Alternative half float libs:
// https://half.sourceforge.net/
// https://github.com/Maratyszcza/FP16
// https://fgiesen.wordpress.com/2012/03/28/half-to-float-done-quic/

// The *wild* code below is from SO:
// https://stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion/60047308#60047308

// IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0,
// +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
using float16 = uint16_t;

inline uint32_t bitsAsUint(const float x) {
	return *(uint32_t*)&x;
}

inline float bitsAsFloat(const uint32_t x) {
	return *(float*)&x;
}

inline float float16ToFloat(float16 x) {
	const uint32_t e = (x & 0x7C00) >> 10;		   // exponent
	const uint32_t m = (x & 0x03FF) << 13;		   // mantissa
	const uint32_t v = bitsAsUint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
	return bitsAsFloat((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) |
					   ((e == 0) & (m != 0)) *
						   ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}

inline float16 floatToFloat16(float x) {
	const uint32_t b = bitsAsUint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const uint32_t e = (b & 0x7F800000) >> 23;	   // exponent
	const uint32_t m =
		b &
		0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) |
		   ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) |
		   (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

} // namespace sgd
