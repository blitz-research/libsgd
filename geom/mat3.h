#pragma once

#include "vec3.h"

namespace sgd {

template <class T> struct Mat3;
template <class T> using CMat3 = const Mat3<T>&;

template <class T> struct Quat;
template <class T> using CQuat = const Quat<T>&;

using Mat3f = Mat3<float>;
using CMat3f = CMat3<float>;

template <class T> struct Mat3 {

	Vec3<T> i{1, 0, 0};
	Vec3<T> j{0, 1, 0};
	Vec3<T> k{0, 0, 1};

	constexpr Mat3() = default;
	constexpr explicit Mat3(T s);
	constexpr Mat3(CVec3<T> i, CVec3<T> j, CVec3<T> k);

	static Mat3 yaw(T r);
	static Mat3 pitch(T r);
	static Mat3 roll(T r);
	static Mat3 rotation(CVec3<T> r);
	static Mat3 rotation(CQuat<T> q);
	static Mat3 scale(CVec3<T> r);

	Vec3<T>* data();
	const Vec3<T>* data() const;

	Vec3<T>& operator[](size_t index);
	CVec3<T> operator[](size_t index) const;

	Mat3& operator*=(CMat3<T> m);
};

template <class T> Mat3<T> operator*(CMat3<T> m, CMat3<T> n);
template <class T> Vec3<T> operator*(CMat3<T> m, CVec3<T> v);

template <class T> bool operator==(CMat3<T> m, CMat3<T> n);
template <class T> bool operator!=(CMat3<T> m, CMat3<T> n);

template <class T> std::ostream& operator<<(std::ostream& os, CMat3<T> m);

template <class T> T yaw(CMat3<T> v);
template <class T> T pitch(CMat3<T> v);
template <class T> T roll(CMat3<T> v);
template <class T> Vec3<T> rotation(CMat3<T> v);
template <class T> Vec3<T> scale(CMat3<T> v);

template <class T> Mat3<T> transpose(CMat3<T> m);
template <class T> Mat3<T> cofactor(CMat3<T> m);
template <class T> Mat3<T> inverse(CMat3<T> m);

} // namespace sgd

#include "mat3.inl"
