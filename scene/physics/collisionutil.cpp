#include "collisionutil.h"

#include "../scene/scene.h"

namespace sgd {

SGD_SHARED(Camera);

template <class IntersectFunc>
Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, IntersectFunc intersectFunc, CollisionResponse response,
				 Vector<Collision>& collisions) {

	static constexpr real mind = (real).0001;
	static constexpr real eps = (real).01;

	static constexpr int maxLoops = 6;

	auto dir = dst - src;
	auto dist = length(dir);
	if (dist <= mind) return src;

	auto dist_xz = length(Vec2r(dir.x, dir.z));
	dir /= dist;
	auto fwd = dir;

	Planer planes[3];
	int state = 0;
	int loops = 0;

	for (;;) {

		Contact contact(dist);
		Line ray(src, dir);

		auto collider = intersectFunc(ray, contact);
		if (!collider) return dst;

		collisions.emplace_back(ray, contact, collider);

		if (response == CollisionResponse::ignore) return dst;

		// Hit distance/time
		auto hitd = std::max(contact.time - eps, (real)0);
		src = ray * hitd;

		if (response == CollisionResponse::stop) return src;

		if (hitd > dist) SGD_LOG << "### OOPS 1:" << hitd << dist;
		if ((dist -= hitd) <= mind) return src;

		// Subtract hit distance moved from remaining distance
		if (response == CollisionResponse::slidexz) {
			auto d_xz = length(Vec2r(src.x - ray.o.x, src.z - ray.o.z));
			if (d_xz > dist_xz) SGD_LOG << "### OOPS 2:" << d_xz << dist_xz;
			dist_xz -= d_xz;
		}

		// Avoid looping forever
		if (++loops == maxLoops) {
#if SGD_CONFIG_DEBUG
			SGD_LOG << "collideRay loops == maxLoops! ray:" << ray << "contact:" << contact;
#endif
			return src;
		}

		Plane plane(src, contact.normal);
		// Plane plane(contact.point, contact.normal);
		// plane.d -= mind;

		// New destination?
		dst = nearest(plane, dst);

		switch (state) {
		case 0: { // First plane hit
			planes[0] = plane;
			state = 1;
			break;
		}
		case 1: { // Second plane hit
			if (distance(planes[0], dst) > 0) {
				planes[0] = plane;
				break;
			}
			// Slide along crease, intersection of 2 planes.
			planes[1] = plane;
			auto crease = normalize(cross(planes[0].n, planes[1].n));
			dst = src + crease * dot(dst - src, crease);
			state = 2;
			break;
		}
		case 2: { // Third plane hit
			if (distance(planes[0], dst) > 0 && distance(planes[1], dst) > 0) {
				planes[0] = plane;
				state = 1;
				break;
			}
			// Stop at point, intersection of 3 planes.
			return src;
		}
		default:
			SGD_ABORT();
		}

		dir = dst - src;
		if (dot(fwd, dir) < 0) return src;

		auto d = length(dir);
		if (d <= mind) return src;

		dir /= d;
		if (d > dist) {
			SGD_LOG << "### OOPS 3:" << d << dist;
			dst = src + dir * dist;
		} else {
			dist = d;
		}

		if (response == CollisionResponse::slidexz) {
			auto d_xz = length(Vec2r(dst.x - src.x, dst.z - src.z));
			if (d_xz > dist_xz) {
				dist *= dist_xz / d_xz;
				if (dist <= mind) return src;
				dst = src + dir * dist;
			} else {
				dist_xz = d_xz;
			}
		}
	}
}

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, float radius, uint32_t colliderMask,
				 CollisionResponse response, Vector<Collision>& collisions) {

	auto intersectFunc = [=](CLiner ray, Contact& contact) -> Collider* {
		return space->intersectRay(ray, radius, colliderMask, contact);
	};

	return collideRay(space, src, dst, intersectFunc, response, collisions);
}

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, CVec3f radii, uint32_t colliderMask,
				 CollisionResponse response, Vector<Collision>& collisions) {

	auto intersectFunc = [=](CLiner ray, Contact& contact) -> Collider* {
		return space->intersectRay(ray, radii, colliderMask, contact);
	};

	return collideRay(space, src, dst, intersectFunc, response, collisions);
}

Collider* intersectRay(Camera* camera, CVec2f windowCoords, int colliderMask, Contact& rcontact) {

	auto wCoords = windowCoords / Vec2f(camera->viewportSize()) * 2.0f - 1.0f;

	auto clipCoords = camera->inverseProjectionMatrix() * Vec4f(wCoords.x, -wCoords.y, 0, 1);

	auto viewPos = clipCoords.xyz() / clipCoords.w;

	auto worldPos = camera->worldMatrix() * Vec3r(viewPos);

	auto r = camera->far() - camera->near();
	auto d = std::sqrt(viewPos.x * viewPos.x + viewPos.y * viewPos.y + r * r);
	Contact contact(d);

	Liner ray(worldPos, normalize(worldPos - camera->worldPosition()));

	auto collider = camera->scene()->collisionSpace()->intersectRay(ray, 0, colliderMask, contact);
	if (collider) rcontact = contact;

	return collider;
}

} // namespace sgd
