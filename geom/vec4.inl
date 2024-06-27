#include "vec4.h"

namespace sgd {

template <class T> constexpr Vec4<T>::Vec4(T s) : x(s), y(s), z(s), w(s) {
}

template <class T> template <class C> constexpr Vec4<T>::Vec4(const Vec4<C>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(v.w)) {
}

template <class T> constexpr Vec4<T>::Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {
}

template <class T> constexpr Vec4<T>::Vec4(const Vec3<T>& v, T w) : x(v.x), y(v.y), z(v.z), w(w) {
}

template <class T> constexpr Vec4<T>::Vec4(const Vec2<T>& v, T z, T w) : x(v.x), y(v.y), z(z), w(w) {
}

template <class T> Vec4<T> Vec4<T>::rgba(uint32_t rgba) {
	constexpr T sc = T(1) / T(255);
	return {T(rgba & 255) * sc, T((rgba >> 8) & 255) * sc, T((rgba >> 16) & 255) * sc, T(rgba >> 24) * sc};
}

template<class T> Vec2<T>& Vec4<T>::xy() {
	return (Vec2<T>&)(*this);
}

template<class T> Vec2<T> Vec4<T>::xy() const {
	return *(Vec2<T>*)this;
}

template<class T> Vec3<T>& Vec4<T>::xyz() {
	return (Vec3<T>&)(*this);
}

template<class T> Vec3<T> Vec4<T>::xyz() const {
	return *(Vec3<T>*)this;
}

template<class T> Vec4<T>::operator Vec3<T>() const {
	return {x,y,z};
}

// ***** Non-members *****

template <class T> Vec4<T> operator-(CVec4<T> v) {
	return {-v.x, -v.y, -v.z, -v.w};
}

template <class T> Vec4<T> operator*(CVec4<T> p, CVec4<T> q) {
	return {p.x * q.x, p.y * q.y, p.z * q.z, p.w * q.w};
}

template <class T> Vec4<T> operator/(CVec4<T> p, CVec4<T> q) {
	return {p.x / q.x, p.y / q.y, p.z / q.z, p.w / q.w};
}

template <class T> Vec4<T> operator+(CVec4<T> p, CVec4<T> q) {
	return {p.x + q.x, p.y + q.y, p.z + q.z, p.w + q.w};
}

template <class T> Vec4<T> operator-(CVec4<T> p, CVec4<T> q) {
	return {p.x - q.x, p.y - q.y, p.z - q.z, p.w - q.w};
}

template <class T> Vec4<T> operator*(CVec4<T> v, T s) {
	return {v.x * s, v.y * s, v.z * s, v.w * s};
}

template <class T> Vec4<T> operator/(CVec4<T> v, T s) {
	return {v.x / s, v.y / s, v.z / s, v.w / s};
}

template <class T> Vec4<T> operator+(CVec4<T> v, T s) {
	return {v.x + s, v.y + s, v.z + s, v.w + s};
}

template <class T> Vec4<T> operator-(CVec4<T> v, T s) {
	return {v.x - s, v.y - s, v.z - s, v.w - s};
}

template <class T> Vec4<T> operator*(T s, CVec4<T> v) {
	return {s * v.x, s * v.y, s * v.z, s * v.w};
}

template <class T> Vec4<T> operator/(T s, CVec4<T> v) {
	return {s / v.x, s / v.y, s / v.z, s / v.w};
}

template <class T> Vec4<T> operator+(T s, CVec4<T> v) {
	return {s + v.x, s + v.y, s + v.z, s + v.w};
}

template <class T> Vec4<T> operator-(T s, CVec4<T> v) {
	return {s - v.x, s - v.y, s - v.z, s - v.w};
}

template <class T> bool operator==(CVec4<T> p, CVec4<T> q) {
	return p.x == q.x && p.y == q.y && p.z == q.z && p.w == q.w;
}

template <class T> bool operator!=(CVec4<T> lhs, CVec4<T> rhs) {
	return !operator==(lhs, rhs);
}

template <class T> bool operator<(CVec4<T> p, CVec4<T> q) {
	if (p.x < q.x) return true;
	if (q.x < p.x) return false;
	if (p.y < q.y) return true;
	if (q.y < p.y) return false;
	if (p.z < q.z) return true;
	if (q.z < p.z) return false;
	return p.w < q.w;
}

template <class T> std::ostream& operator<<(std::ostream& str, CVec4<T>& v) {
	return str << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
}

template <class T> uint32_t rgba(CVec4<T> color) {
	return (uint8_t(color.w * 255) << 24) | (uint8_t(color.z * 255) << 16) | (uint8_t(color.y * 255) << 8) |
		   uint8_t(color.x * 255);
}

} // namespace sgd
