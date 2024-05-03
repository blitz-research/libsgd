#pragma once

#include "plane.h"

#include "util.h"

namespace sgd {

template <class T> Plane<T>::Plane(CVec3<T> n, T d) : n(n), d(d) {
	SGD_ASSERT(approxOne(length(n)));
}

template <class T> Plane<T>::Plane(CVec3<T> p0, CVec3<T> n) : n(n), d(-dot(p0, n)) {
}

template <class T>
Plane<T>::Plane(CVec3<T> v0, CVec3<T> v1, CVec3<T> v2) : n(normalize(cross(v1 - v0, v2 - v0))), d(-dot(v0, n)) {
}

// ***** Non-member operators ****

template <class T> bool operator==(CPlane<T> p, CPlane<T> q) {
	return p.n == q.n && p.d == q.d;
}

template <class T> bool operator!=(CPlane<T> p, CPlane<T> q) {
	return !(p == q);
}

template <class T> std::ostream& operator<<(std::ostream& os, CPlane<T> p) {
	return os << '(' << p.n << ',' << p.d << ')';
}

template <class T> T distance(CPlane<T> p, CVec3<T> v) {
	return dot(p.n, v) + p.d;
}

template <class T> Vec3<T> nearest(CPlane<T> p, CVec3<T> v) {
	return v - p.n * distance(p, v);
}

template <class T> T t_intersect(CPlane<T> p, CLine<T>& l) {
	return -distance(p, l.o) / dot(p.n, l.d);
}

template <class T> Vec3<T> intersect(CPlane<T> p, CLine<T>& l) {
	return l * t_intersect(p, l);
}

} // namespace sgd
