#include "geom/exports.h"

using namespace sgd;

float epsilon = 1e-3f;

int N = 10000;

Vec3f rndT() {
	return {rnd(-1000, 1000), rnd(-1000, 1000), rnd(-1000, 1000)};
}

Vec3f rndR() {
	// Not sure what happens outside this range...
	return {rnd(-halfPi, halfPi), rnd(-pi, pi), rnd(-pi, pi)};
}

Vec3f rndS() {
	return {rnd(.5f, 2), rnd(.5f, 2), rnd(.5f, 2)};
}

bool same(float x, float y) {
	return std::abs(x - y) <= epsilon;
}

bool same(const float* x, const float* y, int n) {
	for (int i = 0; i < n; ++i) {
		if (!same(x[i], y[i])) return false;
	}
	return true;
}

bool same(CVec3f x, CVec3f y) {
	if (same(&x.x, &y.x, 3)) return true;
	log() << "### OOPS:";
	log() << "###" << x;
	log() << "###" << y;
	return false;
}

bool same(CVec4f x, CVec4f y) {
	if (same(&x.x, &y.x, 4)) return true;
	log() << "### OOPS:";
	log() << "###" << x;
	log() << "###" << y;
	return false;
}

bool same(CMat3f x, CMat3f y) {
	log() << "### OOPS:";
	log() << "###" << x;
	log() << "###" << y;
	return false;
}

bool same(CAffineMat4f x, CAffineMat4f y) {
	if (same(&x.r.i.x, &y.r.i.x, 12)) return true;
	log() << "### OOPS:";
	log() << "###" << x;
	log() << "###" << y;
	return false;
}

bool same(CMat4f x, CMat4f y) {
	if (same(&x.i.x, &y.i.x, 16)) return true;
	log() << "### OOPS:";
	log() << "###" << x;
	log() << "###" << y;
	return false;
}

bool same(CQuatf x, CQuatf y) {
	if(same(&x.v.x, &y.v.x, 4)) return true;
	Quatf ny(-y.v, -y.w);
	if(same(&x.v.x, &ny.v.x, 4)) return true;
	log() << "### OOPS:";
	log() << "###" << x;
	log() << "###" << y;
	return false;
}

void testRotConversions() {
	log() << "Testing rot conversions";
	for (int i = 0; i < N; ++i) {

		Vec3f e0 = rndR();

		Mat3f m0 = Mat3f::rotation(e0);
		same(m0, Mat3f::yaw(e0.y) * Mat3f::pitch(e0.x) * Mat3f::roll(e0.z));
		same(e0, rotation(m0));

		Quatf q0 = Quatf::rotation(e0);
		same(q0, Quatf::yaw(e0.y) * Quatf::pitch(e0.x) * Quatf::roll(e0.z));
		same(q0, Quatf::rotation(m0));
		same(m0, Mat3f::rotation(q0));

		Vec3f tv = Vec3f(rnd(-1, 1), rnd(-1, 1), rnd(-1, 1));
		same(m0 * tv, q0 * tv);

		Vec3f e1 = rndR();
		Mat3f m1 = Mat3f::rotation(e1) * m0;
		Quatf q1 = Quatf::rotation(e1) * q0;
		same(m1, Mat3f::rotation(q1));
		same(q1, Quatf::rotation(m1));
	}
}

void testMat3f() {
	log() << "Testing Mat3f";
	for (int i = 0; i < N; ++i) {

		Vec3f r = rndR();
		Vec3f s = rndS();

		Mat3f m0 = Mat3f::rotation(r) * Mat3f::scale(s);
		same(rotation(m0), r);
		same(scale(m0), s);

		Mat3f im = inverse(m0);
		same(im * m0, Mat3f{});

		Vec3f v = rndT();
		same(im * (m0 * v), v);
	}
}

void testMat4f() {
	log() << "Testing Mat4f";
	for (int i = 0; i < N; ++i) {

		Vec3f t = rndT();
		Vec3f r = rndR();
		Vec3f s = rndS();

		Mat4f m0 = Mat4f(AffineMat4f::TRS(t, r, s));

		Mat4f im = inverse(m0);
		same(im * m0, Mat4f{});

		Vec4f v = Vec4f(rndT(), 1);
		same(im * (m0 * v), v);
	}
}

void testAffineMat4f() {
	log() << "Testing AffineMat4f";
	for (int i = 0; i < N; ++i) {

		Vec3f t = rndT();
		Vec3f r = rndR();
		Vec3f s = rndS();

		AffineMat4f m0 = AffineMat4f::TRS(t, r, s);
		same(translation(m0), t);
		same(rotation(m0), r);
		same(scale(m0), s);

		AffineMat4f im = inverse(m0);
		same(im * m0, AffineMat4f{});

		Vec3f v = rndT();
		same(im * (m0 * v), v);
	}
}

void testRectf() {
	log() << "Testing Rectf";
	for (int i = 0; i < N; ++i) {}
}

void testVec2f() {
	log() << "Testing Vec2f";
	for (int i = 0; i < N; ++i) {

		Vec2f v0 = Vec2f(rnd(.1f, 1), rnd(.1f, 1));
		same(length(normalize(v0)), 1.0f);
	}
}

void testBoxf() {
	log() << "Testing Boxf";
	for (int i = 0; i < N; ++i) {}
}

int main() {

	testVec2f();
	testRectf();
	testMat3f();
	testMat4f();
	testAffineMat4f();
	testBoxf();

	testRotConversions();
}
