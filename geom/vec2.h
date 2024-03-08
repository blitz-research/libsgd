#pragma once

#include "core/exports.h"

namespace sgd {

template <class T> struct Vec2;
template <class T> using CVec2 = const Vec2<T>&;

using Vec2f = Vec2<float>;
using CVec2f = CVec2<float>;

using Vec2i = Vec2<int32_t>;
using CVec2i = CVec2<int32_t>;

using Vec2u = Vec2<uint32_t>;
using CVec2u = CVec2<uint32_t>;

template <class T> struct Vec2 {

	T x{};
	T y{};

	constexpr Vec2() = default;
	constexpr explicit Vec2(T s);
	template <class C> constexpr explicit Vec2(const Vec2<C>& v);
	constexpr Vec2(T x, T y);

	Vec2& operator*=(CVec2<T> v);
	Vec2& operator/=(CVec2<T> v);
	Vec2& operator+=(CVec2<T> v);
	Vec2& operator-=(CVec2<T> v);

	Vec2& operator*=(T s);
	Vec2& operator/=(T s);
	Vec2& operator+=(T s);
	Vec2& operator-=(T s);
};

template <class T> Vec2<T> operator-(CVec2<T> v);

template <class T> Vec2<T> operator*(CVec2<T> p, CVec2<T> q);
template <class T> Vec2<T> operator/(CVec2<T> p, CVec2<T> q);
template <class T> Vec2<T> operator+(CVec2<T> p, CVec2<T> q);
template <class T> Vec2<T> operator-(CVec2<T> p, CVec2<T> q);

template <class T> Vec2<T> operator*(CVec2<T> v, T s);
template <class T> Vec2<T> operator/(CVec2<T> v, T s);
template <class T> Vec2<T> operator+(CVec2<T> v, T s);
template <class T> Vec2<T> operator-(CVec2<T> v, T s);

template <class T> Vec2<T> operator*(T s, CVec2<T> v);
template <class T> Vec2<T> operator/(T s, CVec2<T> v);
template <class T> Vec2<T> operator+(T s, CVec2<T> v);
template <class T> Vec2<T> operator-(T s, CVec2<T> v);

template <class T> bool operator==(CVec2<T> p, CVec2<T> q);
template <class T> bool operator!=(CVec2<T> p, CVec2<T> q);
template <class T> bool operator<(CVec2<T> p, CVec2<T> q);

template <class T> std::ostream& operator<<(std::ostream& str, CVec2<T> v);

template <class T> T length(CVec2<T> v);
template <class T> Vec2<T> normalize(CVec2<T> v);
template <class T> T dot(CVec2<T> p, CVec2<T> q);

} // namespace sgd

#include "vec2.inl"
