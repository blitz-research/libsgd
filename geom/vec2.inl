#pragma once

#include "vec2.h"

namespace sgd {

template <class T> constexpr Vec2<T>::Vec2(T s) : x(s), y(s) {
}

template <class T> constexpr Vec2<T>::Vec2(T x, T y) : x(x), y(y) {
}

template <class T> template <class C> constexpr Vec2<T>::Vec2(CVec2<C>& v) : x(T(v.x)), y(T(v.y)) {
}

template <class T> Vec2<T>& Vec2<T>::operator*=(CVec2<T> v) {
	return *this = *this * v;
}

template <class T> Vec2<T>& Vec2<T>::operator/=(CVec2<T> v) {
	return *this = *this / v;
}

template <class T> Vec2<T>& Vec2<T>::operator+=(CVec2<T> v) {
	return *this = *this + v;
}

template <class T> Vec2<T>& Vec2<T>::operator-=(CVec2<T> v) {
	return *this = *this - v;
}

template <class T> Vec2<T>& Vec2<T>::operator*=(T s) {
	return *this = *this * s;
}

template <class T> Vec2<T>& Vec2<T>::operator/=(T s) {
	return *this = *this / s;
}

template <class T> Vec2<T>& Vec2<T>::operator+=(T s) {
	return *this = *this + s;
}

template <class T> Vec2<T>& Vec2<T>::operator-=(T s) {
	return *this = *this - s;
}

// ***** Non-member operators *****

template <class T> Vec2<T> operator-(CVec2<T> v) {
	return {-v.x, -v.y};
}

template <class T> Vec2<T> operator*(CVec2<T> p, CVec2<T> q) {
	return {p.x * q.x, p.y * q.y};
}

template <class T> Vec2<T> operator/(CVec2<T> p, CVec2<T> q) {
	return {p.x / q.x, p.y / q.y};
}

template <class T> Vec2<T> operator+(CVec2<T> p, CVec2<T> q) {
	return {p.x + q.x, p.y + q.y};
}

template <class T> Vec2<T> operator-(CVec2<T> p, CVec2<T> q) {
	return {p.x - q.x, p.y - q.y};
}

template <class T> Vec2<T> operator*(CVec2<T> v, T s) {
	return {v.x * s, v.y * s};
}

template <class T> Vec2<T> operator/(CVec2<T> v, T s) {
	return {v.x / s, v.y / s};
}

template <class T> Vec2<T> operator+(CVec2<T> v, T s) {
	return {v.x + s, v.y + s};
}

template <class T> Vec2<T> operator-(CVec2<T> v, T s) {
	return {v.x - s, v.y - s};
}

template <class T> Vec2<T> operator*(T s, CVec2<T> v) {
	return {s * v.x, s * v.y};
}

template <class T> Vec2<T> operator/(T s, CVec2<T> v) {
	return {s / v.x, s / v.y};
}

template <class T> Vec2<T> operator+(T s, CVec2<T> v) {
	return {s + v.x, s + v.y};
}

template <class T> Vec2<T> operator-(T s, CVec2<T> v) {
	return {s - v.x, s - v.y};
}

template <class T> bool operator==(CVec2<T> p, CVec2<T> q) {
	return p.x == q.x && p.y == q.y;
}

template <class T> bool operator!=(CVec2<T> p, CVec2<T> q) {
	return !operator==(p, q);
}

template <class T> bool operator<(CVec2<T> p, CVec2<T> q) {
	if (p.x < q.x) return true;
	if (q.x < p.x) return false;
	return p.y < q.y;
}

template <class T> std::ostream& operator<<(std::ostream& str, CVec2<T> v) {
	return str << "Vec2(" << v.x << ", " << v.y << ")";
}

// ***** Non-member functions *****

template <class T> T length(CVec2<T> v) {
	return std::sqrt(v.x * v.x + v.y * v.y);
}

template <class T> T dot(CVec2<T> p, CVec2<T> q) {
	return p.x * q.x + p.y * q.y;
}

template <class T> Vec2<T> normalize(CVec2<T> v) {
	T d = length(v);
	return d ? v / d : v;
}

} // namespace sgd
