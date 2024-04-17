#pragma once

#include "vec3.h"

namespace sgd {

template <class T> constexpr Vec3<T>::Vec3(T s) : x(s), y(s), z(s) {
}

template <class T> template <class C> constexpr Vec3<T>::Vec3(CVec3<C>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {
}

template <class T> constexpr Vec3<T>::Vec3(CVec2<T> v, float z) : x(v.x), y(v.y), z(z) {
}

template <class T> constexpr Vec3<T>::Vec3(T x, T y, T z) : x(x), y(y), z(z) {
}

template <class T> Vec3<T>& Vec3<T>::operator*=(CVec3<T> v) {
	return *this = *this * v;
}

template <class T> Vec3<T>& Vec3<T>::operator/=(CVec3<T> v) {
	return *this = *this / v;
}

template <class T> Vec3<T>& Vec3<T>::operator+=(CVec3<T> v) {
	return *this = *this + v;
}

template <class T> Vec3<T>& Vec3<T>::operator-=(CVec3<T> v) {
	return *this = *this - v;
}

template <class T> Vec3<T>& Vec3<T>::operator*=(T s) {
	return *this = *this * s;
}

template <class T> Vec3<T>& Vec3<T>::operator/=(T s) {
	return *this = *this / s;
}

template <class T> Vec3<T>& Vec3<T>::operator+=(T s) {
	return *this = *this + s;
}

template <class T> Vec3<T>& Vec3<T>::operator-=(T s) {
	return *this = *this - s;
}

template<class T> Vec3<T>::operator Vec2<T>() const {
	return {x,y};
}

// ***** Non-member operators *****

template <class T> Vec3<T> operator-(CVec3<T> v) {
	return {-v.x, -v.y, -v.z};
}

template <class T> Vec3<T> operator*(CVec3<T> p, CVec3<T> q) {
	return {p.x * q.x, p.y * q.y, p.z * q.z};
}

template <class T> Vec3<T> operator/(CVec3<T> p, CVec3<T> q) {
	return {p.x / q.x, p.y / q.y, p.z / q.z};
}

template <class T> Vec3<T> operator+(CVec3<T> p, CVec3<T> q) {
	return {p.x + q.x, p.y + q.y, p.z + q.z};
}

template <class T> Vec3<T> operator-(CVec3<T> p, CVec3<T> q) {
	return {p.x - q.x, p.y - q.y, p.z - q.z};
}

template <class T> Vec3<T> operator*(CVec3<T> v, T s) {
	return {v.x * s, v.y * s, v.z * s};
}

template <class T> Vec3<T> operator/(CVec3<T> v, T s) {
	return {v.x / s, v.y / s, v.z / s};
}

template <class T> Vec3<T> operator+(CVec3<T> v, T s) {
	return {v.x + s, v.y + s, v.z + s};
}

template <class T> Vec3<T> operator-(CVec3<T> v, T s) {
	return {v.x - s, v.y - s, v.z - s};
}

template <class T> Vec3<T> operator*(T s, CVec3<T> v) {
	return {s * v.x, s * v.y, s * v.z};
}

template <class T> Vec3<T> operator/(T s, CVec3<T> v) {
	return {s / v.x, s / v.y, s / v.z};
}

template <class T> Vec3<T> operator+(T s, CVec3<T> v) {
	return {s + v.x, s + v.y, s + v.z};
}

template <class T> Vec3<T> operator-(T s, CVec3<T> v) {
	return {s - v.x, s - v.y, s - v.z};
}

template <class T> bool operator==(CVec3<T> p, CVec3<T> q) {
	return p.x == q.x && p.y == q.y && p.z == q.z;
}

template <class T> bool operator!=(CVec3<T> p, CVec3<T> q) {
	return !operator==(p, q);
}

template <class T> bool operator<(CVec3<T> p, CVec3<T> q) {
	if (p.x < q.x) return true;
	if (q.x < p.x) return false;
	if (p.y < q.y) return true;
	if (q.y < p.y) return false;
	return p.z < q.z;
}

template <class T> std::ostream& operator<<(std::ostream& str, CVec3<T> v) {
	return str << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}

// ***** Non-member functions *****

template <class T> Vec3<T> min(CVec3<T> p, CVec3<T> q) {
	return {std::min(p.x, q.x), std::min(p.y, q.y), std::min(p.z, q.z)};
}

template <class T> Vec3<T> max(CVec3<T> p, CVec3<T> q) {
	return {std::max(p.x, q.x), std::max(p.y, q.y), std::max(p.z, q.z)};
}

template <class T> T length(CVec3<T> v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template <class T> T yaw(CVec3<T> v) {
	return -std::atan2(v.x, v.z);
}

template <class T> T pitch(CVec3<T> v) {
	return std::atan2(v.y, std::sqrt(v.x * v.x + v.z * v.z));
}

template <class T> T dot(CVec3<T> p, CVec3<T> q) {
	return p.x * q.x + p.y * q.y + p.z * q.z;
}

template <class T> Vec3<T> cross(CVec3<T> p, CVec3<T> q) {
	return {p.y * q.z - p.z * q.y, p.z * q.x - p.x * q.z, p.x * q.y - p.y * q.x};
}

template <class T> Vec3<T> normalize(CVec3<T> v) {
	T d = length(v);
	return d ? v / d : v;
}

} // namespace sgd
