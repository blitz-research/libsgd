#pragma once

#include "mat4.h"

namespace sgd {

// ***** Members *****

template <class T> constexpr Mat4<T>::Mat4(T s) : i(s, 0, 0, 0), j(0, s, 0, 0), k(0, 0, s, 0), t(0, 0, 0, s) {
}

template <class T> template <class C> constexpr Mat4<T>::Mat4(CMat4<C> m) : i(m.i), j(m.j), k(m.k), t(m.t) {
}

template <class T>
template <class C>
constexpr Mat4<T>::Mat4(CAffineMat4<C> m) : i(m.r.i, 0), j(m.r.j, 0), k(m.r.k, 0), t(m.t, 1) {
}

template <class T> constexpr Mat4<T>::Mat4(CVec4<T> i, CVec4<T> j, CVec4<T> k, CVec4<T> t) : i(i), j(j), k(k), t(t) {
}

template <class T> Mat4<T> Mat4<T>::ortho(T left, T right, T bottom, T top, T near, T far) {
	T w = right - left, h = top - bottom, td = far - near;
	// clang-format off
	return {
		{2 / w,					0,						0,						0},
		{0,						2 / h,					0,						0},
		{0,						0,						1 / td,					0},
		{-(right + left) / w,	-(top + bottom) / h,	- near / td,	 		1}};
	// clang-format on
	/*
	T w = right - left, h = top - bottom, d = far - near;
	// clang-format off
	return {
		{2 / w,					0,						0,						0},
		{0,						2 / h,					0,						0},
		{0,						0,						2 / d,					0},
		{-(right + left) / w,	-(top + bottom) / h,	-(far + near) / d, 		1}};
	// clang-format on
	 */
}

template <class T> Mat4<T> Mat4<T>::frustum(T left, T right, T bottom, T top, T near, T far) {
	T w = right - left, h = top - bottom, d = far - near, near2 = near * 2, q = far / (far - near);
	// clang-format off
	return {
		{near2 / w,				0,						0,						0},
		{0,						near2 / h,				0,						0},
		{(right + left) / w, 	(top + bottom) / h,		far / d, 				1},
		{0,						0,						-far * near / d, 		0}};
	// clang-format on
}

template <class T> Mat4<T> Mat4<T>::perspective(T fovyInDegrees, T aspectRatio, T near, T far) {

	T ymax = near * std::tan(fovyInDegrees * pi / 360.0);
	T xmax = ymax * aspectRatio;

	return frustum(-xmax, xmax, -ymax, ymax, near, far);
}

template<class T> Mat4<T> Mat4<T>::orthographic(T fovyInDegrees, T aspectRatio, T near, T far) {

	T ymax = near * std::tan(fovyInDegrees * pi / 360.0);
	T xmax = ymax * aspectRatio;

	return ortho(-xmax, xmax, -ymax, ymax, near, far);
}

template <class T> Mat4<T>& Mat4<T>::operator*=(CMat4<T> m) {
	return *this = *this * m;
}

template <class T> Mat4<T>::operator AffineMat4<T>() const {
	return AffineMat4<T>(Mat3f(i.xyz(), j.xyz(), k.xyz()), t.xyz());
}

// ***** Non-member operators *****

template <class T> Vec4<T> operator*(CMat4<T> m, CVec4<T> v) {
	return {m.i.x * v.x + m.j.x * v.y + m.k.x * v.z + m.t.x * v.w, m.i.y * v.x + m.j.y * v.y + m.k.y * v.z + m.t.y * v.w,
			m.i.z * v.x + m.j.z * v.y + m.k.z * v.z + m.t.z * v.w, m.i.w * v.x + m.j.w * v.y + m.k.w * v.z + m.t.w * v.w};
}

template <class T> Mat4<T> operator*(CMat4<T> m, CMat4<T> n) {
	return {m * n.i, m * n.j, m * n.k, m * n.t};
}

template <class T> bool operator==(CMat4<T> m, CMat4<T> n) {
	return m.i == n.i && m.j == n.j && m.k == n.k && m.t == n.t;
}

template <class T> bool operator!=(CMat4<T> m, CMat4<T> n) {
	return !operator==(m, n);
}

template <class T> std::ostream& operator<<(std::ostream& os, CMat4<T> m) {
	return os << '(' << m.i << ',' << m.j << ',' << m.k << ',' << m.t << ')';
}

// ***** Non-member functions *****

template <class T> Mat4<T> transpose(CMat4<T> m) {
	return {{m.i.x, m.j.x, m.k.x, m.t.x}, //
			{m.i.y, m.j.y, m.k.y, m.t.y}, //
			{m.i.z, m.j.z, m.k.z, m.t.z}, //
			{m.i.w, m.j.w, m.k.w, m.t.w}};
}

template <class T> Mat4<T> inverse(CMat4<T> m) {
	Mat4<T> r{{m.j.y * m.k.z * m.t.w - m.j.y * m.k.w * m.t.z - m.k.y * m.j.z * m.t.w + m.k.y * m.j.w * m.t.z +
				   m.t.y * m.j.z * m.k.w - m.t.y * m.j.w * m.k.z,
			   -m.i.y * m.k.z * m.t.w + m.i.y * m.k.w * m.t.z + m.k.y * m.i.z * m.t.w - m.k.y * m.i.w * m.t.z -
				   m.t.y * m.i.z * m.k.w + m.t.y * m.i.w * m.k.z,
			   m.i.y * m.j.z * m.t.w - m.i.y * m.j.w * m.t.z - m.j.y * m.i.z * m.t.w + m.j.y * m.i.w * m.t.z +
				   m.t.y * m.i.z * m.j.w - m.t.y * m.i.w * m.j.z,
			   -m.i.y * m.j.z * m.k.w + m.i.y * m.j.w * m.k.z + m.j.y * m.i.z * m.k.w - m.j.y * m.i.w * m.k.z -
				   m.k.y * m.i.z * m.j.w + m.k.y * m.i.w * m.j.z},

			  {-m.j.x * m.k.z * m.t.w + m.j.x * m.k.w * m.t.z + m.k.x * m.j.z * m.t.w - m.k.x * m.j.w * m.t.z -
				   m.t.x * m.j.z * m.k.w + m.t.x * m.j.w * m.k.z,
			   m.i.x * m.k.z * m.t.w - m.i.x * m.k.w * m.t.z - m.k.x * m.i.z * m.t.w + m.k.x * m.i.w * m.t.z +
				   m.t.x * m.i.z * m.k.w - m.t.x * m.i.w * m.k.z,
			   -m.i.x * m.j.z * m.t.w + m.i.x * m.j.w * m.t.z + m.j.x * m.i.z * m.t.w - m.j.x * m.i.w * m.t.z -
				   m.t.x * m.i.z * m.j.w + m.t.x * m.i.w * m.j.z,
			   m.i.x * m.j.z * m.k.w - m.i.x * m.j.w * m.k.z - m.j.x * m.i.z * m.k.w + m.j.x * m.i.w * m.k.z +
				   m.k.x * m.i.z * m.j.w - m.k.x * m.i.w * m.j.z},

			  {m.j.x * m.k.y * m.t.w - m.j.x * m.k.w * m.t.y - m.k.x * m.j.y * m.t.w + m.k.x * m.j.w * m.t.y +
				   m.t.x * m.j.y * m.k.w - m.t.x * m.j.w * m.k.y,
			   -m.i.x * m.k.y * m.t.w + m.i.x * m.k.w * m.t.y + m.k.x * m.i.y * m.t.w - m.k.x * m.i.w * m.t.y -
				   m.t.x * m.i.y * m.k.w + m.t.x * m.i.w * m.k.y,
			   m.i.x * m.j.y * m.t.w - m.i.x * m.j.w * m.t.y - m.j.x * m.i.y * m.t.w + m.j.x * m.i.w * m.t.y +
				   m.t.x * m.i.y * m.j.w - m.t.x * m.i.w * m.j.y,
			   -m.i.x * m.j.y * m.k.w + m.i.x * m.j.w * m.k.y + m.j.x * m.i.y * m.k.w - m.j.x * m.i.w * m.k.y -
				   m.k.x * m.i.y * m.j.w + m.k.x * m.i.w * m.j.y},

			  {-m.j.x * m.k.y * m.t.z + m.j.x * m.k.z * m.t.y + m.k.x * m.j.y * m.t.z - m.k.x * m.j.z * m.t.y -
				   m.t.x * m.j.y * m.k.z + m.t.x * m.j.z * m.k.y,
			   m.i.x * m.k.y * m.t.z - m.i.x * m.k.z * m.t.y - m.k.x * m.i.y * m.t.z + m.k.x * m.i.z * m.t.y +
				   m.t.x * m.i.y * m.k.z - m.t.x * m.i.z * m.k.y,
			   -m.i.x * m.j.y * m.t.z + m.i.x * m.j.z * m.t.y + m.j.x * m.i.y * m.t.z - m.j.x * m.i.z * m.t.y -
				   m.t.x * m.i.y * m.j.z + m.t.x * m.i.z * m.j.y,
			   m.i.x * m.j.y * m.k.z - m.i.x * m.j.z * m.k.y - m.j.x * m.i.y * m.k.z + m.j.x * m.i.z * m.k.y +
				   m.k.x * m.i.y * m.j.z - m.k.x * m.i.z * m.j.y}};

	T s = T(1) / (m.i.x * r.i.x + m.i.y * r.j.x + m.i.z * r.k.x + m.i.w * r.t.x);

	return {r.i * s, r.j * s, r.k * s, r.t * s};
}

} // namespace sgd
