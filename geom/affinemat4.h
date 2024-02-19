#pragma once

#include "box.h"
#include "line.h"
#include "mat3.h"
#include "plane.h"
#include "vec3.h"

namespace sgd {

template <class T> struct AffineMat4;
template <class T> using CAffineMat4 = const AffineMat4<T>&;

using AffineMat4f = AffineMat4<float>;
using CAffineMat4f = CAffineMat4<float>;

template <class T> struct AffineMat4 {

	Mat3<T> r{};
	Vec3<T> t{};

	constexpr AffineMat4() = default;
	constexpr AffineMat4(CMat3<T> r, CVec3<T> t);

	static AffineMat4 translation(CVec3<T> t);
	static AffineMat4 rotation(CVec3<T> r);
	static AffineMat4 scale(CVec3<T> s);

	static AffineMat4 TRS(CVec3<T> t, CVec3<T> r, CVec3<T> s);
	static AffineMat4 TRS(CVec3<T> t, CVec3<T> r);
	static AffineMat4 TRS(CVec3<T> t);

	AffineMat4& operator*=(CAffineMat4<T> m);
};

template <class T> AffineMat4<T> operator*(CAffineMat4<T> m, CAffineMat4<T> n);
template <class T> Vec3<T> operator*(CAffineMat4<T> m, CVec3<T> v);
template <class T> Box<T> operator*(CAffineMat4<T> m, CBox<T> b);
template <class T> Line<T> operator*(CAffineMat4<T> m, CLine<T> l);
template <class T> Plane<T> operator*(CAffineMat4<T> m, CPlane<T> p);

template <class T> bool operator==(CAffineMat4<T> m, CAffineMat4<T> n);
template <class T> bool operator!=(CAffineMat4<T> m, CAffineMat4<T> n);

template <class T> std::ostream& operator<<(std::ostream& os, CAffineMat4<T> m);

template<class T> Vec3<T> translation(CAffineMat4<T> m);
template<class T> Vec3<T> rotation(CAffineMat4<T> m);
template<class T> Vec3<T> scale(CAffineMat4<T> m);

template <class T> AffineMat4<T> transpose(CAffineMat4<T> m);
template <class T> AffineMat4<T> inverse(CAffineMat4<T> m);

} // namespace sgd

#include "affinemat4.inl"
