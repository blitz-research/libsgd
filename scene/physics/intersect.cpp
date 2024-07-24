#include "intersect.h"

namespace sgd {

namespace {

inline bool isUnit(CVec3r v) {
	return std::abs(dot(v, v) - 1) <= degenerateNormalEpsilon;
}

}

bool intersectRaySphere(CLiner worldRay, CVec3r v0, real radius, Contact& contact) {

	SGD_ASSERT(isUnit(worldRay.d));

	Liner ray = worldRay - v0;

	real b = -dot(ray.o, ray.d);
	real c = dot(ray.o, ray.o) - radius * radius;
	real h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	real t = b - h;
	if (t < 0 || t >= contact.time) return false;

	Vec3r hp = ray * t;

	contact.point = hp + v0;
	contact.normal = hp / radius;
	contact.time = t;
	//	debug() << "### RaySphere" << Contact.normal.length() << Contact.time;

	return true;
}

bool intersectRayCapsule(CLiner worldRay, CVec3r v0, CVec3r dir, real length, real radius, Contact& contact) {

	SGD_ASSERT(isUnit(worldRay.d) && isUnit(dir));

	Liner ray = worldRay - v0;

	Vec3r po = ray.o - dir * dot(ray.o, dir);
	Vec3r pd = normalize(ray.d - dir * dot(ray.d, dir));

	real b = -dot(po, pd);
	real c = dot(po, po) - radius * radius;
	real h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	real t = (b - h);

	Vec3r hp = ray * t;
	real hd = dot(hp, dir);

	if (hd >= 0 && hd <= length) {
		if (t < 0 || t >= contact.time) return false;
		contact.point = hp + v0;
		contact.normal = (po + pd * t) / radius;
		contact.time = t;
		// debug() << "### RayCapsule body" << Contact.normal.length() << Contact.time;

	} else {
		hd = hd > 0 ? length : 0;
		Vec3r org = dir * hd;
		Vec3r v = org - ray.o;
		real b = dot(v, ray.d);
		real c = dot(v, v) - radius * radius;
		real h = b * b - c;
		if (h < 0) return false;

		h = std::sqrt(h);
		real t = (b - h);
		if (t < 0 || t >= contact.time) return false;
		Vec3r hp = ray * t;
		contact.point = hp + v0;
		contact.normal = (hp - org) / radius;
		contact.time = t;
		// debug() << "### RayCapsule cap" << Contact.normal.length() << Contact.time;
	}
	return true;
}

bool intersectRayCylinder(CLiner worldRay, CVec3r v0, CVec3r dir, real length, real radius, Contact& contact) {

	SGD_ASSERT(isUnit(worldRay.d) && isUnit(dir));

	Liner ray = worldRay - v0;

	Vec3r po = ray.o - dir * dot(ray.o, dir);
	Vec3r pd = normalize(ray.d - dir * dot(ray.d, dir));

	real b = -dot(po, pd);
	real c = dot(po, po) - radius * radius;
	real h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	real t = (b - h);

	Vec3r hp = ray * t;
	real hd = dot(hp, dir);

	if (hd >= 0 && hd <= length) {
		if (t < 0 || t >= contact.time) return false;
		contact.point = hp + v0;
		contact.normal = (po + pd * t) / radius;
		contact.time = t;
		// debug() << "### RayCylinder body" << Contact.normal.length() << Contact.time;
	} else {
		// TODO: cylinder caps
		return false;
	}

	return true;
}

bool intersectRayEdge(CLiner worldRay, real radius, CVec3r v0, CVec3r v1, Contact& contact) {

	SGD_ASSERT(isUnit(worldRay.d));

	real dist = length(v1 - v0);
	Vec3r dir = (v1 - v0) / dist;
	Liner ray = worldRay - v0;

	Vec3r po = ray.o - dir * dot(ray.o, dir);
	Vec3r pd = normalize(ray.d - dir * dot(ray.d, dir));

	real b = -dot(po, pd);
	real c = dot(po, po) - radius * radius;
	real h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	real t = (b - h);

	Vec3r hp = ray * t;
	real hd = dot(hp, dir);
	if (hd > dist) return false;

	if (hd >= 0) {
		if (t < 0 || t >= contact.time) return false;
		contact.point = hp + v0;
		contact.normal = (po + pd * t) / radius;
		contact.time = t;
		//		debug() << "### RayTriangle edge cylinder" << Contact.normal.length() << Contact.time;
	} else {
		// Collide with sphere at v0
		real b = -dot(ray.o, ray.d);
		real c = dot(ray.o, ray.o) - radius * radius;
		real h = b * b - c;
		if (h < 0) return false;

		h = std::sqrt(h);
		real t = (b - h);
		if (t < 0 || t >= contact.time) return false;

		Vec3r hp = ray * t;

		contact.point = hp + v0;
		contact.normal = hp / radius;
		contact.time = t;
		//		debug() << "### RayTriangle edge sphere" << Contact.normal.length() << Contact.time;
	}

	return true;
}

bool intersectRayTriangle(CLiner ray, real radius, CVec3r v0, CVec3r v1, CVec3r v2, Contact& contact) {

	SGD_ASSERT(isUnit(ray.d));

	Planer plane(v0, v1, v2);
	if (dot(plane.n, ray.d) >= 0) return false;

	plane.d -= radius;
	real t = t_intersect(plane, ray);
	if (t >= contact.time) return false;

	Vec3r hp = ray * t;

	Planer p0(v0 + plane.n, v1, v0);
	bool f0 = distance(p0, hp) >= 0;

	Planer p1(v1 + plane.n, v2, v1);
	bool f1 = distance(p1, hp) >= 0;

	Planer p2(v2 + plane.n, v0, v2);
	bool f2 = distance(p2, hp) >= 0;

	if (t >= 0 && f0 && f1 && f2) {
		contact.point = hp;
		contact.normal = plane.n;
		contact.time = t;
		// debug() << "### RayTriangle plane" << Contact.normal.length() << Contact.time;
		return true;
	}

	bool collision = false;
	if (!f0) collision |= intersectRayEdge(ray, radius, v0, v1, contact);
	if (!f1) collision |= intersectRayEdge(ray, radius, v1, v2, contact);
	if (!f2) collision |= intersectRayEdge(ray, radius, v2, v0, contact);
	return collision;
}

} // namespace wb
