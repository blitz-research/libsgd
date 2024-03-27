// #include "quat.h"

namespace sgd {

template <class T> constexpr Quat<T>::Quat(CVec3<T> v, T w) : v(v), w(w) {
}

template <class T> Quat<T> Quat<T>::yaw(T r) {
	return {Vec3<T>(0, std::sin(r * .5f), 0), std::cos(r * .5f)};
}

template <class T> Quat<T> Quat<T>::pitch(T r) {
	return {Vec3<T>(std::sin(r * .5f), 0, 0), std::cos(r * .5f)};
}

template <class T> Quat<T> Quat<T>::roll(T r) {
	return {Vec3<T>(0, 0, std::sin(r * .5f)), std::cos(r * .5f)};
}

template <class T> Quat<T> Quat<T>::rotation(CVec3<T> r) {
	T cp = std::cos(r.x * 0.5f), sp = std::sin(r.x * 0.5f);
	T cy = std::cos(r.y * 0.5f), sy = std::sin(r.y * 0.5f);
	T cr = std::cos(r.z * 0.5f), sr = std::sin(r.z * 0.5f);

	return {{sp * cy * cr - cp * sy * sr, //
			 cp * sy * cr + sp * cy * sr, //
			 cp * cy * sr + sp * sy * cr},
			cp * cy * cr - sp * sy * sr};
}

template <class T> Quat<T> Quat<T>::rotation(CMat3<T> m) {
	// https://web.archive.org/web/20231102225313/https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
	Quat<T> q;
	T t;
	if (m.k.z < 0) {
		if (m.i.x > m.j.y) {
			t = 1 + m.i.x - m.j.y - m.k.z;
			q.v = {t, m.i.y + m.j.x, m.k.x + m.i.z};
			q.w = (m.j.z - m.k.y);
		} else {
			t = 1 - m.i.x + m.j.y - m.k.z;
			q.v = {m.i.y + m.j.x, t, m.j.z + m.k.y};
			q.w = (m.k.x - m.i.z);
		}
	} else {
		if (m.i.x < -m.j.y) {
			t = 1 - m.i.x - m.j.y + m.k.z;
			q.v = {m.k.x + m.i.z, m.j.z + m.k.y, t};
			q.w = (m.i.y - m.j.x);
		} else {
			t = 1 + m.i.x + m.j.y + m.k.z;
			q.v = {m.j.z - m.k.y, m.k.x - m.i.z, m.i.y - m.j.x};
			q.w = t;
		}
	}
	if (std::acos(q.w) < halfPi) {
		q.v = -q.v;
	} else {
		q.w = -q.w;
	}
	T s = T(.5) / std::sqrt(t);
	q.v *= s;
	q.w *= s;
	return q;
}

template <class T> Quat<T>& Quat<T>::operator*=(CQuat<T> q) {
	return *this = *this * q;
}

template <class T> Quat<T>& Quat<T>::operator+=(CQuat<T> q) {
	return *this = *this + q;
}

template <class T> Quat<T>& Quat<T>::operator*=(T s) {
	return *this = *this * s;
}

template <class T> Quat<T>& Quat<T>::operator/=(T s) {
	return *this = *this / s;
}

// ***** Non-member operstor *****

template <class T> Quat<T> operator-(CQuat<T> q) {
	return {-q.v, q.w};
}

template <class T> Quat<T> operator*(CQuat<T> q, CQuat<T> r) {
	return {cross(r.v, q.v) + r.v * q.w + q.v * r.w, r.w * q.w - dot(r.v, q.v)}; // Note: Flipped lhs/rhs to match matrix muls.
}

template <class T> Quat<T> operator+(CQuat<T> q, CQuat<T> r) {
	return {q.v + r.v, q.w + r.w};
}

template <class T> Quat<T> operator*(CQuat<T> q, T s) {
	return {q.v * s, q.w * s};
}

template <class T> Quat<T> operator/(CQuat<T> q, T s) {
	return {q.v / s, q.w / s};
}

template <class T> Vec3<T> operator*(CQuat<T> q, CVec3<T> v) {
	return (q * Quat<T>(v, 0) * -q).v;
}

template <class T> std::ostream& operator<<(std::ostream& str, CQuat<T> q) {
	return str << '(' << q.v << ',' << q.w << ')';
}

template <class T> T dot(CQuat<T> q, CQuat<T> r) {
	return dot(q.v, r.v) + q.w * r.w;
}

template <class T> T length(CQuat<T> q) {
	return std::sqrt(dot(q, q));
}

template <class T> Quat<T> normalize(CQuat<T> q) {
	return q / length(q);
}

template <class T> Quat<T> slerp(CQuat<T> q, CQuat<T> r, float alpha) {
	constexpr T epsilon = 0;

	float beta = 1 - alpha;
	float d = dot(q, r);
	auto t = r;
	if (d < 0) {
		t.v = -t.v;
		t.w = -t.w;
		d = -d;
	}

	if (d < 1 - epsilon) {
		float om = std::acos(d);
		float si = std::sin(om);
		alpha = std::sin(alpha * om) / si;
		beta = std::sin(beta * om) / si;
	}
	return q * beta + t * alpha;
}

} // namespace sgd
