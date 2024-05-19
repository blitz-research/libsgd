#pragma once

#include "vec2.h"

namespace sgd {

template <class T> struct Vec3;
template <class T> using CVec3 = const Vec3<T>&;

using Vec3f = Vec3<float>;
using CVec3f = CVec3<float>;

using Vec3r = Vec3<real>;
using CVec3r = CVec3<real>;

template <class T> struct Vec3 {

	T x{};
	T y{};
	T z{};

	constexpr Vec3() = default;
	constexpr explicit Vec3(T s);
	template <class C> constexpr Vec3(CVec3<C>& v); // NOLINT (non-explicit ctor)
	constexpr Vec3(CVec2<T> v, float z);
	constexpr Vec3(T x, T y, T z);

	Vec2<T>& xy();
	CVec2<T> xy() const;

	explicit operator Vec2<T>() const;

	Vec3& operator*=(CVec3<T> v);
	Vec3& operator/=(CVec3<T> v);
	Vec3& operator+=(CVec3<T> v);
	Vec3& operator-=(CVec3<T> v);

	Vec3& operator*=(T s);
	Vec3& operator/=(T s);
	Vec3& operator+=(T s);
	Vec3& operator-=(T s);
};

template <class T> Vec3<T> operator-(CVec3<T> v);

template <class T> Vec3<T> operator*(CVec3<T> p, CVec3<T> q);
template <class T> Vec3<T> operator/(CVec3<T> p, CVec3<T> q);
template <class T> Vec3<T> operator+(CVec3<T> p, CVec3<T> q);
template <class T> Vec3<T> operator-(CVec3<T> p, CVec3<T> q);

template <class T> Vec3<T> operator*(CVec3<T> v, T s);
template <class T> Vec3<T> operator/(CVec3<T> v, T s);
template <class T> Vec3<T> operator+(CVec3<T> v, T s);
template <class T> Vec3<T> operator-(CVec3<T> v, T s);

template <class T> Vec3<T> operator*(T s, CVec3<T> v);
template <class T> Vec3<T> operator/(T s, CVec3<T> v);
template <class T> Vec3<T> operator+(T s, CVec3<T> v);
template <class T> Vec3<T> operator-(T s, CVec3<T> v);

template <class T> bool operator==(CVec3<T> p, CVec3<T> q);
template <class T> bool operator!=(CVec3<T> p, CVec3<T> q);
template <class T> bool operator<(CVec3<T> p, CVec3<T> q);

template <class T> std::ostream& operator<<(std::ostream& str, CVec3<T> v);

template <class T> Vec3<T> min(CVec3<T> p, CVec3<T> q);
template <class T> Vec3<T> max(CVec3<T> p, CVec3<T> q);

template <class T> T length(CVec3<T> v);
template <class T> T lengthsq(CVec3<T> v);
template <class T> T yaw(CVec3<T> v);
template <class T> T pitch(CVec3<T> v);

template <class T> T dot(CVec3<T> p, CVec3<T> q);
template <class T> Vec3<T> cross(CVec3<T> p, CVec3<T> q);
template <class T> Vec3<T> normalize(CVec3<T> v);

} // namespace sgd

#include "vec3.inl"
