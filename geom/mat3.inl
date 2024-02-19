//#include "mat3.h"
#include "quat.h"

namespace sgd {

template <class T> constexpr Mat3<T>::Mat3(T s) : i(s), j(s), k(s) {
}

template <class T> constexpr Mat3<T>::Mat3(CVec3<T> i, CVec3<T> j, CVec3<T> k) : i(i), j(j), k(k) {
}

template <class T> Mat3<T> Mat3<T>::yaw(T r) {
	T s = std::sin(r), c = std::cos(r);
	return {{c, 0, s}, {0, 1, 0}, {-s, 0, c}};
}

template <class T> Mat3<T> Mat3<T>::pitch(T r) {
	T s = std::sin(r), c = std::cos(r);
	return {{1, 0, 0}, {0, c, -s}, {0, s, c}};
}

template <class T> Mat3<T> Mat3<T>::roll(T r) {
	T s = std::sin(r), c = std::cos(r);
	return {{c, -s, 0}, {s, c, 0}, {0, 0, 1}};
}

template <class T> Mat3<T> Mat3<T>::rotation(CVec3<T> r) {
	return yaw(r.y) * pitch(r.x) * roll(r.z);
}

template <class T> Mat3<T> Mat3<T>::rotation(CQuat<T> q) {
	T xx = q.v.x * q.v.x, yy = q.v.y * q.v.y, zz = q.v.z * q.v.z;
	T xy = q.v.x * q.v.y, xz = q.v.x * q.v.z, yz = q.v.y * q.v.z;
	T wx = q.w * q.v.x, wy = q.w * q.v.y, wz = q.w * q.v.z;
	return {{1 - 2 * (yy + zz), 2 * (xy - wz), 2 * (xz + wy)},
			{2 * (xy + wz), 1 - 2 * (xx + zz), 2 * (yz - wx)},
			{2 * (xz - wy), 2 * (yz + wx), 1 - 2 * (xx + yy)}};
}

template <class T> Mat3<T> Mat3<T>::scale(CVec3<T> s) {
	return {{s.x, 0, 0}, {0, s.y, 0}, {0, 0, s.z}};
}

template <class T> Vec3<T>* Mat3<T>::data() {
	return &i;
}

template <class T> const Vec3<T>* Mat3<T>::data() const {
	return &i;
}

template <class T> Vec3<T>& Mat3<T>::operator[](size_t index) {
	SGD_ASSERT(index < 3);
	return data()[index];
}

template <class T> CVec3<T> Mat3<T>::operator[](size_t index) const {
	SGD_ASSERT(index < 3);
	return data()[index];
}

template <class T> Mat3<T>& Mat3<T>::operator*=(CMat3<T> m) {
	return *this = *this * m;
}

// ***** Non-member operators *****

template <class T> Mat3<T> operator*(CMat3<T> m, CMat3<T> n) {
	return {{m.i.x * n.i.x + m.j.x * n.i.y + m.k.x * n.i.z, m.i.y * n.i.x + m.j.y * n.i.y + m.k.y * n.i.z,
			 m.i.z * n.i.x + m.j.z * n.i.y + m.k.z * n.i.z},
			{m.i.x * n.j.x + m.j.x * n.j.y + m.k.x * n.j.z, m.i.y * n.j.x + m.j.y * n.j.y + m.k.y * n.j.z,
			 m.i.z * n.j.x + m.j.z * n.j.y + m.k.z * n.j.z},
			{m.i.x * n.k.x + m.j.x * n.k.y + m.k.x * n.k.z, m.i.y * n.k.x + m.j.y * n.k.y + m.k.y * n.k.z,
			 m.i.z * n.k.x + m.j.z * n.k.y + m.k.z * n.k.z}};
}

template <class T> Vec3<T> operator*(CMat3<T> m, CVec3<T> v) {
	return {m.i.x * v.x + m.j.x * v.y + m.k.x * v.z, m.i.y * v.x + m.j.y * v.y + m.k.y * v.z,
			m.i.z * v.x + m.j.z * v.y + m.k.z * v.z};
}

template <class T> bool operator==(CMat3<T> m, CMat3<T> n) {
	return m.i == n.i && m.j == n.j && m.k == n.k;
}

template <class T> bool operator!=(CMat3<T> m, CMat3<T> n) {
	return !(m == n);
}

template <class T> std::ostream& operator<<(std::ostream& os, CMat3<T> m) {
	return os << '(' << m.i << ',' << m.j << ',' << m.k << ')';
}

// ***** Non-member functions *****

template <class T> T yaw(CMat3<T> m) {
	return yaw(m.k);
}

template <class T> T pitch(CMat3<T> m) {
	return pitch(m.k);
}

template <class T> T roll(CMat3<T> m) {
	return -std::atan2(m.i.y / length(m.i), m.j.y / length(m.j));
}

template <class T> Vec3<T> rotation(CMat3<T> m) {
	return {pitch(m), yaw(m), roll(m)};
}

template <class T> Vec3<T> scale(CMat3<T> m) {
	return {length(m.i), length(m.j), length(m.k)};
}

template <class T> Mat3<T> transpose(CMat3<T> m) {
	return {{m.i.x, m.j.x, m.k.x}, {m.i.y, m.j.y, m.k.y}, {m.i.z, m.j.z, m.k.z}};
}

template <class T> Mat3<T> cofactor(CMat3<T> m) {
	return {{(m.j.y * m.k.z - m.j.z * m.k.y), -(m.j.x * m.k.z - m.j.z * m.k.x), (m.j.x * m.k.y - m.j.y * m.k.x)},
			{-(m.i.y * m.k.z - m.i.z * m.k.y), (m.i.x * m.k.z - m.i.z * m.k.x), -(m.i.x * m.k.y - m.i.y * m.k.x)},
			{(m.i.y * m.j.z - m.i.z * m.j.y), -(m.i.x * m.j.z - m.i.z * m.j.x), (m.i.x * m.j.y - m.i.y * m.j.x)}};
}

template <class T> Mat3<T> inverse(CMat3<T> m) {
	Mat3<T> r{{(m.j.y * m.k.z - m.j.z * m.k.y), -(m.i.y * m.k.z - m.i.z * m.k.y), (m.i.y * m.j.z - m.i.z * m.j.y)},
			  {-(m.j.x * m.k.z - m.j.z * m.k.x), (m.i.x * m.k.z - m.i.z * m.k.x), -(m.i.x * m.j.z - m.i.z * m.j.x)},
			  {(m.j.x * m.k.y - m.j.y * m.k.x), -(m.i.x * m.k.y - m.i.y * m.k.x), (m.i.x * m.j.y - m.i.y * m.j.x)}};

	T s = T(1) / (m.i.x * r.i.x + m.i.y * r.j.x + m.i.z * r.k.x);

	return {r.i * s, r.j * s, r.k * s};
}

} // namespace sgd
