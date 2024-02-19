#pragma once

#include "vec3.h"

namespace sgd {

template <class T> struct Line;
template <class T> using CLine = const Line<T>&;

using Linef = Line<float>;
using CLinef = CLine<float>;

template <class T> struct Line {

	Vec3<T> o{};
	Vec3<T> d{};

	Line() = default;
	Line(CVec3<T> o, CVec3<T> d);

	Line& operator+=(CVec3<T> v);
	Line& operator-=(CVec3<T> v);
};

template <class T> Vec3<T> operator*(CLine<T> l, T t);
template <class T> Line<T> operator+(CLine<T> l, CVec3<T> v);
template <class T> Line<T> operator-(CLine<T> l, CVec3<T> v);

template <class T> std::ostream& operator<<(std::ostream& os, CLine<T> l);

template <class T> Line<T> normalize(CLine<T> l);
template <class T> T t_nearest(CLine<T> l, CVec3<T> v);
template <class T> Vec3<T> nearest(CLine<T> l, CVec3<T> v);

} // namespace sgd

#include "line.inl"
