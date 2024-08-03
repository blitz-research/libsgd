#include "collisionutil.h"

#include "../scene/scene.h"

namespace sgd {

SGD_SHARED(Camera);

template <class IntersectFunc>
Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, IntersectFunc intersectFunc, CollisionResponse response,
				 Vector<Collision>& collisions) {

	static constexpr real eps = (real).001;
	static constexpr int maxLoops = 6;

	Planer hitPlanes[3];
	int state = 0;
	int loops = 0;

	auto maxd = std::numeric_limits<real>::max();

	auto fwd = dst - src;

	for (;;) {

		auto dir = dst - src;
		if (dot(dir, fwd) < 0) return src;
		auto d = length(dir);
		if (d <= eps) return src;
		dir /= d;
		if (d > maxd) {
			if(state) SGD_LOG << "### d > maxd";
			dst = src + dir * maxd;
		} else {
			maxd = d;
		}

		Contact contact(maxd);
		Line ray(src, dir);

		auto collider = intersectFunc(ray, contact);
		if (!collider) return dst;

		collisions.emplace_back(ray, contact, collider);

		if (response == CollisionResponse::ignore) return dst;

		// Hit distance/time
		auto hitd = std::max(contact.time - eps, (real)0);
		src = ray * hitd;

		if (response == CollisionResponse::stop) return src;

		// Subtract hit distance moved from remaining distance
		if ((maxd -= hitd) <= eps) return src;

		// Avoid looping forever
		if (++loops == maxLoops) {
#if SGD_CONFIG_DEBUG
			SGD_LOG << "collideRay nHits == maxLoops! ray:" << ray << "contact:" << contact;
#endif
			return src;
		}

		Plane plane(src, contact.normal);
//		Plane plane(contact.point, contact.normal);
//		plane.d -= eps;

		// New destination?
		dst = nearest(plane, dst);

		switch (state) {
		case 0: { // First plane hit
			hitPlanes[0] = plane;
			state = 1;
			break;
		}
		case 1: { // Second plane hit
			if (distance(hitPlanes[0], dst) > 0) {
				hitPlanes[0] = plane;
				break;
			}
			// Slide along crease, intersection of 2 planes.
			hitPlanes[1] = plane;
			auto crease = normalize(cross(hitPlanes[0].n, hitPlanes[1].n));
			dst = src + crease * dot(dst - src, crease);
			state = 2;
			break;
		}
		case 2: { // Third plane hit
			if (distance(hitPlanes[0], dst) > 0 && distance(hitPlanes[1], dst) > 0) {
				hitPlanes[0] = plane;
				state = 1;
				break;
			}
			// Stop at point, intersection of 3 planes.
			return src;
		}
		default:
			SGD_ABORT();
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
