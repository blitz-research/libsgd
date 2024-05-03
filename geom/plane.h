#pragma once

#include "line.h"
#include "vec3.h"

#include <limits>

namespace sgd {

template <class T> struct Plane;
template <class T> using CPlane = const Plane<T>&;

using Planef = Plane<float>;
using CPlanef = CPlane<float>;

using Planer = Plane<real>;
using CPlaner = CPlane<real>;

template <class T> struct Plane {

	Vec3<T> n{0, 1, 0};
	T d{};

	Plane() = default;
	Plane(CVec3<T> n, T d);
	Plane(CVec3<T> p, CVec3<T> n);				  // Point, normal
	Plane(CVec3<T> v0, CVec3<T> v1, CVec3<T> v2); // vertices
};

template <class T> bool operator==(CPlane<T> p, CPlane<T> q);
template <class T> bool operator!=(CPlane<T> p, CPlane<T> q);

template <class T> std::ostream& operator<<(std::ostream& os, CPlane<T> p);

template <class T> T distance(CPlane<T> p, CVec3<T> v);
template <class T> T t_intersect(CPlane<T> p, CLine<T>& l);
template <class T> Vec3<T> nearest(CPlane<T> p, CVec3<T> v);
template <class T> Vec3<T> intersect(CPlane<T> p, CLine<T>& l);

} // namespace sgd

#include "plane.inl"
