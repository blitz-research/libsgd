#pragma once

#include "affinemat4.h"

namespace sgd {

template <class T> constexpr AffineMat4<T>::AffineMat4(CMat3<T> r, CVec3<T> t) : r(r), t(t) {
}

template <class T> AffineMat4<T> AffineMat4<T>::translation(CVec3<T> t) {
	return {{}, t};
}

template <class T> AffineMat4<T> AffineMat4<T>::rotation(CVec3<T> r) {
	return {Mat3<T>::rotation(r), {}};
}

template <class T> AffineMat4<T> AffineMat4<T>::scale(CVec3<T> s) {
	return {Mat3<T>::scale(s), {}};
}

template <class T> AffineMat4<T> AffineMat4<T>::TRS(CVec3<T> t, CVec3<T> r, CVec3<T> s) {
	return translation(t) * rotation(r) * scale(s);
}

template <class T> AffineMat4<T> AffineMat4<T>::TRS(CVec3<T> t, CVec3<T> r) {
	return translation(t) * rotation(r);
}

template <class T> AffineMat4<T> AffineMat4<T>::TRS(CVec3<T> t) {
	return translation(t);
}

template <class T> AffineMat4<T>& AffineMat4<T>::operator*=(CAffineMat4<T> m) {
	return *this = *this * m;
}

// ***** Non-member operators *****

template <class T> AffineMat4<T> operator*(CAffineMat4<T> m, CAffineMat4<T> n) {
	return {m.r * n.r, m.r * n.t + m.t};
}

template <class T> Vec3<T> operator*(CAffineMat4<T> m, CVec3<T> v) {
	return m.r * v + m.t;
}

template <class T> Box<T> operator*(CAffineMat4<T> m, CBox<T> b) {
	Box<T> r;
	for (int i = 0; i < 8; ++i) r |= m * b.corner(i);
	return r;
}

template <class T> Line<T> operator*(CAffineMat4<T> m, CLine<T> l) {
	return {m * l.o, m * l.d};
}

template <class T> Plane<T> operator*(CAffineMat4<T> m, CPlane<T> p) {
	return {m * (p.n * -p.d), m * p.n};
}

template <class T> bool operator==(CAffineMat4<T> m, CAffineMat4<T> n) {
	return m.r == n.r && m.t == n.t;
}

template <class T> bool operator!=(CAffineMat4<T> m, CAffineMat4<T> n) {
	return !(m == n);
}

template <class T> std::ostream& operator<<(std::ostream& os, CAffineMat4<T> m) {
	return os << '(' << m.r << ',' << m.t << ')';
}

// ***** Non-member functions *****

template <class T> Vec3<T> translation(CAffineMat4<T> m) {
	return m.t;
}

template <class T> Vec3<T> rotation(CAffineMat4<T> m) {
	return rotation(m.r);
}

template <class T> Vec3<T> scale(CAffineMat4<T> m) {
	return scale(m.r);
}

template <class T> AffineMat4<T> transpose(CAffineMat4<T> m) {
	auto i = transpose(m.r);
	return {i, i * -m.t};
}

template <class T> AffineMat4<T> inverse(CAffineMat4<T> m) {
	auto i = inverse(m.r);
	return {i, i * -m.t};
}

} // namespace sgd
