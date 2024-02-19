#pragma once

#include "line.h"
#include "vec3.h"

#include <limits>

namespace sgd {

template <class T> struct Box;
template <class T> using CBox = const Box<T>&;

using Boxf = Box<float>;
using CBoxf = CBox<float>;

template <class T> struct Box {

	Vec3<T> min{std::numeric_limits<T>::max()};
	Vec3<T> max{std::numeric_limits<T>::lowest()};

	constexpr Box() = default;
	constexpr explicit Box(CVec3<T> v);
	constexpr Box(float min, float max);
	constexpr Box(CVec3<T> min, CVec3<T> max);

	Box& operator|=(CBox<T> b);
	Box& operator&=(CBox<T> b);
	Box& operator|=(CVec3<T> v);
	Box& operator&=(CVec3<T> v);
	Box& operator+=(CVec3<T> v);
	Box& operator-=(CVec3<T> v);
};

template <class T> Box<T> operator&(CBox<T> b, CBox<T> c);
template <class T> Box<T> operator|(CBox<T> b, CBox<T> c);

template <class T> Box<T> operator&(CBox<T> b, CVec3<T> v);
template <class T> Box<T> operator|(CBox<T> b, CVec3<T> v);
template <class T> Box<T> operator+(CBox<T> b, CVec3<T> v);
template <class T> Box<T> operator-(CBox<T> b, CVec3<T> v);

template<class T> bool operator==(CBox<T> b, CBox<T> c);
template<class T> bool operator!=(CBox<T> b, CBox<T> c);
template<class T> bool operator<(CBox<T> b, CBox<T> c);

template<class T> std::ostream& operator<<(std::ostream& os, CBox<T> b);

template<class T> bool empty(CBox<T> b);
template<class T> Vec3<T> size(CBox<T> b);
template<class T> Vec3<T> center(CBox<T> b);
template<class T> Vec3<T> corner(CBox<T> b,int index);

} // namespace sgd

#include "box.inl"
