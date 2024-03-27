#pragma once

#include "line.h"

namespace sgd {

template <class T> Line<T>::Line(CVec3<T> o, CVec3<T> d) : o(o), d(d) {
}

template <class T> Line<T>& Line<T>::operator+=(CVec3<T> v) {
	return *this = *this + v;
}

template <class T> Line<T>& Line<T>::operator-=(CVec3<T> v) {
	return *this = *this - v;
}

// ***** Non-member operators *****

template <class T> Vec3<T> operator*(CLine<T> l, T t) {
	return l.o + l.d * t;
}

template <class T> Line<T> operator+(CLine<T> l, CVec3<T> v) {
	return {l.o + v, l.d};
}

template <class T> Line<T> operator-(CLine<T> l, CVec3<T> v) {
	return {l.o - v, l.d};
}

template <class T> std::ostream& operator<<(std::ostream& os, CLine<T> l) {
	return os << '(' << l.o << ',' << l.d << ')';
}

// ***** Non-member functions

template <class T> Line<T> normalize(CLine<T> l) {
	return Line(l.o, normalize(l.d));
}

template <class T> T t_nearest(CLine<T> l, CVec3<T> v) {
	return l.d.dot(v - l.o) / l.d.dot(l.d);
}

template <class T> Vec3<T> nearest(CLine<T> l, CVec3<T> v) {
	return l * t_nearest(l, v);
}

} // namespace sgd
