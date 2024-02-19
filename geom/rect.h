#pragma once

#include "vec2.h"

#include <limits>

namespace sgd {

template <class T> struct Rect;
template <class T> using CRect = const Rect<T>&;

using Rectf = Rect<float>;
using CRectf = CRect<float>;

using Recti = Rect<int32_t>;
using CRecti = CRect<int32_t>;

using Rectu = Rect<uint32_t>;
using CRectu = CRect<uint32_t>;

template <class T> struct Rect {

	Vec2<T> min{std::numeric_limits<T>::max()};
	Vec2<T> max{std::numeric_limits<T>::min()};

	constexpr Rect() = default;
	constexpr explicit Rect(CVec2<T> v);
	template <class C> constexpr explicit Rect(CRect<C> r);
	constexpr Rect(CVec2<T> min, CVec2<T> max);
	constexpr Rect(T x0, T y0, T x1, T y1);

	Rect& operator&=(CRect<T> r);
	Rect& operator|=(CRect<T> r);
	Rect& operator+=(CVec2<T> v);
	Rect& operator-=(CVec2<T> v);

	bool empty() const;
	Vec2<T> size() const;
};

template <class T> Rect<T> operator&(CRect<T> r, CRect<T> s);
template <class T> Rect<T> operator|(CRect<T> r, CRect<T> s);
template <class T> Rect<T> operator+(CRect<T> r, CVec2<T> v);
template <class T> Rect<T> operator-(CRect<T> r, CVec2<T> v);

template <class T> bool operator==(CRect<T> r, CRect<T> s);
template <class T> bool operator!=(CRect<T> r, CRect<T> s);

template <class T> std::ostream& operator<<(std::ostream& os, CRect<T>& r);

} // namespace sgd

#include "rect.inl"
