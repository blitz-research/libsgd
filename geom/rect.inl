#include "rect.h"

namespace sgd {

template <class T> constexpr Rect<T>::Rect(CVec2<T> v) : min(v), max(v) {
}

template <class T> template <class C> constexpr Rect<T>::Rect(CRect<C> r) : min(r.min), max(r.max) {
}

template <class T> constexpr Rect<T>::Rect(CVec2<T> min, CVec2<T> max) : min(min), max(max) {
}

template <class T> constexpr Rect<T>::Rect(T x0, T y0, T x1, T y1) : min(x0, y0), max(x1, y1) {
}

template <class T> Rect<T>& Rect<T>::operator&=(CRect<T> r) {
	return *this = *this & r;
}

template <class T> Rect<T>& Rect<T>::operator|=(CRect<T> r) {
	return *this = *this | r;
}

template <class T> Rect<T>& Rect<T>::operator+=(CVec2<T> v) {
	return *this = *this + v;
}

template <class T> Rect<T>& Rect<T>::operator-=(CVec2<T> v) {
	return *this = *this - v;
}

template <class T> bool Rect<T>::empty() const {
	return max.x <= min.x || max.y <= min.y;
}

template <class T> Vec2<T> Rect<T>::size() const {
	return max - min;
}

// ***** Non-members *****

template <class T> Rect<T> operator&(CRect<T> r, CRect<T> s) {
	return {std::max(r.min.x, s.min.x), std::max(r.min.y, s.min.y), //
			std::min(r.max.x, s.max.x), std::min(r.max.y, s.max.y)};
}

template <class T> Rect<T> operator|(CRect<T> r, CRect<T> s) {
	return {std::min(r.min.x, s.min.x), std::min(r.min.y, s.min.y), //
			std::max(r.max.x, s.max.x), std::max(r.max.y, s.max.y)};
}

template <class T> Rect<T> operator+(CRect<T> r, CVec2<T> v) {
	return {r.min + v, r.max + v};
}

template <class T> Rect<T> operator-(CRect<T> r, CVec2<T> v) {
	return {r.min - v, r.max - v};
}

template <class T> bool operator==(CRect<T> r, CRect<T> s) {
	return r.min == s.min && r.max == s.max;
}

template <class T> bool operator!=(CRect<T> r, CRect<T> s) {
	return !(r == s);
}

template <class T> std::ostream& operator<<(std::ostream& os, CRect<T>& r) {
	return os << "Rect(" << r.min.x << ',' << r.min.y << ',' << r.max.x << ',' << r.max.y << ')';
}

} // namespace sgd
