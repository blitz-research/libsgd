#pragma once

#include "vec3.h"

namespace sgd {

template <class T> struct Vec4;
template <class T> using CVec4 = const Vec4<T>&;

using Vec4f = Vec4<float>;
using CVec4f = CVec4<float>;

template <class T> struct Vec4 {

	T x{};
	T y{};
	T z{};
	T w{};

	constexpr Vec4() = default;
	constexpr explicit Vec4(T s);
	template <class C> constexpr explicit Vec4(const Vec4<C>& v);
	constexpr Vec4(const Vec3<T>& v, T w);
	constexpr Vec4(T x, T y, T z, T w);

	static Vec4 rgba(uint32_t rgba);

	T* data();
	const T* data() const;

	T& operator[](size_t index);
	const T& operator[](size_t index) const;

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

} // namespace sgd

#include "vec4.inl"
