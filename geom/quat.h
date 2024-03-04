#pragma once

#include "mat3.h"

namespace sgd {

using Quatf = Quat<float>;
using CQuatf = CQuat<float>;

template <class T> struct Quat {

	Vec3<T> v{};
	T w{1};

	constexpr Quat() = default;
	constexpr Quat(CVec3<T> v, T w);

	static Quat yaw(T r);
	static Quat pitch(T r);
	static Quat roll(T r);
	static Quat rotation(CVec3<T> r);
	static Quat rotation(CMat3<T> m);

	Quat& operator*=(CQuat<T> q);
	Quat& operator+=(CQuat<T> q);
	Quat& operator*=(T s);
	Quat& operator/=(T s);
};

template<class T> Quat<T> operator-(CQuat<T> q);

template<class T> Quat<T> operator*(CQuat<T> q, CQuat<T> r);
template<class T> Quat<T> operator+(CQuat<T> q, CQuat<T> r);
template<class T> Quat<T> operator*(CQuat<T> q, T s);
template<class T> Quat<T> operator/(CQuat<T> q, T s);

template<class T> Vec3<T> operator*(CQuat<T> q, CVec3<T> v);

template<class T> std::ostream& operator<<(std::ostream& os, CQuat<T> q);

template <class T> T length(CQuat<T> q);
template <class T> Quat<T> normalize(CQuat<T> q);
template <class T> T dot(CQuat<T> q, CQuat<T> r);

template<class T> Quat<T> slerp(CQuat<T> q, CQuat<T> r, float alpha);

} // namespace sgd

#include "quat.inl"
