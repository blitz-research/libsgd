#pragma once

#include "vec3.h"

namespace sgd {

template <class T> struct Vec4;
template <class T> using CVec4 = const Vec4<T>&;

using Vec4f = Vec4<float>;
using CVec4f = CVec4<float>;

using Vec4r = Vec4<real>;
using CVec4r = CVec4<real>;

template <class T> struct Vec4 {

	T x{};
	T y{};
	T z{};
	T w{};

	constexpr Vec4() = default;
	constexpr explicit Vec4(T s);
	template <class C> constexpr Vec4(const Vec4<C>& v); // NOLINT (non-explicit ctor)
	constexpr Vec4(const Vec3<T>& v, T w);
	constexpr Vec4(const Vec2<T>& v, T z, T w);
	constexpr Vec4(T x, T y, T z, T w);

	static Vec4 rgba(uint32_t rgba);

	Vec2<T>& xy();
	Vec2<T> xy() const;
	Vec3<T>& xyz();
	Vec3<T> xyz() const;

	explicit operator Vec3<T>() const;

	Vec4& operator*=(CVec4<T> v);
	Vec4& operator/=(CVec4<T> v);
	Vec4& operator+=(CVec4<T> v);
	Vec4& operator-=(CVec4<T> v);

	Vec4& operator*=(T s);
	Vec4& operator/=(T s);
	Vec4& operator+=(T s);
	Vec4& operator-=(T s);
};

template <class T> Vec4<T> operator-(CVec4<T> v);

template <class T> Vec4<T> operator*(CVec4<T> p, CVec4<T> q);
template <class T> Vec4<T> operator/(CVec4<T> p, CVec4<T> q);
template <class T> Vec4<T> operator+(CVec4<T> p, CVec4<T> q);
template <class T> Vec4<T> operator-(CVec4<T> p, CVec4<T> q);

template <class T> Vec4<T> operator*(CVec4<T> v, T s);
template <class T> Vec4<T> operator/(CVec4<T> v, T s);
template <class T> Vec4<T> operator+(CVec4<T> v, T s);
template <class T> Vec4<T> operator-(CVec4<T> v, T s);

template <class T> Vec4<T> operator*(T s, CVec4<T> v);
template <class T> Vec4<T> operator/(T s, CVec4<T> v);
template <class T> Vec4<T> operator+(T s, CVec4<T> v);
template <class T> Vec4<T> operator-(T s, CVec4<T> v);

template <class T> bool operator==(CVec4<T> p, CVec4<T> q);
template <class T> bool operator!=(CVec4<T> lhs, CVec4<T> rhs);
template <class T> bool operator<(CVec4<T> p, CVec4<T> q);

template <class T> std::ostream& operator<<(std::ostream& str, CVec4<T>& v);

template <class T> uint32_t rgba(CVec4<T> color);

template <class T> Vec4<T> toLinearColor(CVec4<T> color);

template <class T> Vec4<T> toSRGBAColor(CVec4<T> color);

} // namespace sgd

#include "vec4.inl"
