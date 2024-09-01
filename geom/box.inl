#pragma once

#include "box.h"

namespace sgd {

template <class T> constexpr Box<T>::Box(CVec3<T> v) : min(v), max(v) {
}

template <class T> constexpr Box<T>::Box(CVec3<T> min, CVec3<T> max) : min(min), max(max) {
}

template <class T> constexpr Box<T>::Box(float min, float max) : min(min), max(max) {
}

template <class T> template <class C> constexpr Box<T>::Box(CBox<C>& b) : min(b.min), max(b.max) {
}

template <class T> Box<T>& Box<T>::operator&=(CBox<T> b) {
	return *this = *this & b;
}

template <class T> Box<T>& Box<T>::operator|=(CBox<T> b) {
	return *this = *this | b;
}

template <class T> Box<T>& Box<T>::operator&=(CVec3<T> v) {
	return *this = *this & v;
}

template <class T> Box<T>& Box<T>::operator|=(CVec3<T> v) {
	return *this = *this | v;
}

template <class T> Box<T>& Box<T>::operator+=(CVec3<T> v) {
	return *this = *this + v;
}

template <class T> Box<T>& Box<T>::operator-=(CVec3<T> v) {
	return *this = *this - v;
}

// ***** Non-member operatorss *****

template <class T> Box<T> operator&(CBox<T> b, CBox<T> c) {
	return {sgd::max(b.min, c.min), sgd::min(b.max, c.max)};
}

template <class T> Box<T> operator&(CBox<T> b, CVec3<T> v) {
	return {sgd::max(b.min, v), sgd::min(b.max, v)};
}

template <class T> Box<T> operator|(CBox<T> b, CBox<T> c) {
	return {sgd::min(b.min, c.min), sgd::max(b.max, c.max)};
}

template <class T> Box<T> operator|(CBox<T> b, CVec3<T> v) {
	return {sgd::min(b.min, v), sgd::max(b.max, v)};
}

template <class T> Box<T> operator+(CBox<T> b, CVec3<T> v) {
	return {b.min + v, b.max + v};
}

template <class T> Box<T> operator-(CBox<T> b, CVec3<T> v) {
	return {b.min - v, b.max - v};
}

template <class T> bool operator==(CBox<T> b, CBox<T> c) {
	return b.min == c.min && b.max == c.max;
}

template <class T> bool operator!=(CBox<T> b, CBox<T> c) {
	return !operator==(b, c);
}

template <class T> bool operator<(CBox<T> b, CBox<T> c) {
	if (b.min < c.min) return true;
	if (c.min < b.min) return false;
	return b.max < c.max;
}

template <class T> std::ostream& operator<<(std::ostream& os, CBox<T> b) {
	return os << '(' << b.min << ',' << b.max << ')';
}

// ***** Non-member functions *****

template <class T> bool empty(CBox<T> b) {
	return b.max.x < b.min.x || b.max.y < b.min.y || b.max.z < b.min.z;
}

template<class T> T volume(CBox<T> b) {
	return (b.max.x-b.min.x) * (b.max.y-b.min.y) * (b.max.z-b.min.z);
}

template <class T> Vec3<T> size(CBox<T> b) {
	return b.max - b.min;
}

template <class T> Vec3<T> center(CBox<T> b) {
	return (b.max - b.min) / T(2) + b.min;
}

template <class T> Vec3<T> corner(CBox<T> b, int index) {
	return {(index & 1) ? b.max.x : b.min.x, (index & 2) ? b.max.y : b.min.y, (index & 4) ? b.max.z : b.min.z};
}

template <class T> bool intersects(CBox<T>& b, CBox<T>& c) {
	return c.min.x < b.max.x && c.max.x > b.min.x && //
		   c.min.y < b.max.y && c.max.y > b.min.y && //
		   c.min.z < b.max.z && c.max.z > b.min.z;
}

template <class T> bool contains(CBox<T>& b, CBox<T>& c) {
	return c.min.x >= b.min.x && c.max.x <= b.max.x && //
		   c.min.y >= b.min.y && c.max.y <= b.max.y && //
		   c.min.z >= b.min.z && c.max.z <= b.max.z;
}

} // namespace sgd
