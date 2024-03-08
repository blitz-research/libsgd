#pragma once

#include "affinemat4.h"
#include "mat3.h"
#include "vec4.h"

namespace sgd {

template <class T> struct Mat4;
template <class T> using CMat4 = const Mat4<T>&;

using Mat4f = Mat4<float>;
using CMat4f = CMat4<float>;

template <class T> struct Mat4 {

	Vec4<T> i{1, 0, 0, 0};
	Vec4<T> j{0, 1, 0, 0};
	Vec4<T> k{0, 0, 1, 0};
	Vec4<T> t{0, 0, 0, 1};

	constexpr Mat4() = default;
	constexpr explicit Mat4(T s);
	constexpr explicit Mat4(CAffineMat4<T> m);
	constexpr Mat4(CVec4<T> i, CVec4<T> j, CVec4<T> k, CVec4<T> t);

	static Mat4 ortho(T left, T right, T bottom, T top, T near, T far);
	static Mat4 frustum(T left, T right, T bottom, T top, T near, T far);
	static Mat4 perspective(T fovyInDegrees, T aspectRatio, T near, T far);

	Mat4& operator*=(CMat4<T> m);

	explicit operator AffineMat4<T>() const;
};

template <class T> Mat4<T> operator*(CMat4<T> m, CMat4<T> n);
template <class T> Vec4<T> operator*(CMat4<T> m, CVec4<T> v);

template <class T> bool operator==(CMat4<T> m, CMat4<T> n);
template <class T> bool operator!=(CMat4<T> m, CMat4<T> n);

template <class T> std::ostream& operator<<(std::ostream& os, CMat4<T> m);

template <class T> Mat4<T> transpose(CMat4<T> m);
template <class T> Mat4<T> inverse(CMat4<T> m);

} // namespace sgd

#include "mat4.inl"
