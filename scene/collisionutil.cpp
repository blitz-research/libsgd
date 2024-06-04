#include "collisionutil.h"

#include "camera.h"
#include "scene.h"

namespace sgd {

SGD_SHARED(Camera);

template <class IntersectFunc>
Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, IntersectFunc intersectFunc, CollisionResponse response,
				 Vector<Collision>& collisions) {

	static constexpr int maxLoops = 6;
	static constexpr real eps = (real).0001;

	// Max distance to travel
	auto dir = dst - src;
	auto d = length(dir);
	if (d <= eps) return src;

	Planer hitPlanes[3];
	int nHits = 0;
	int i = 0;

	for (;;) {

		Contact contact(d);

		Line ray(src, normalize(dst - src));

		auto collider = intersectFunc(ray, contact);
		if (!collider) return dst;

		collisions.emplace_back(ray, contact, collider);
		if (response == CollisionResponse::ignore) return dst;

		auto t = std::max(contact.time - eps, (real)0);
		src = ray * t;

		if (response == CollisionResponse::stop) return src;

		if (++nHits == maxLoops) {
#if SGD_CONFIG_DEBUG
			SGD_LOG << "collideRay nHits == maxLoops! ray:" << ray << "contact:" << contact;
#endif
			return src;
		}

		auto ndot = dot(contact.normal, ray.d);
		if (ndot == (real)-1) return src;

		d -= t;
		if (d <= eps) return src;

		hitPlanes[i] = {contact.point, contact.normal};
		hitPlanes[i].d -= eps;

		dst = nearest(hitPlanes[i], dst);

		switch (i) {
		case 0: {
			// First hit, move along plane
			i = 1;
			break;
		}
		case 1: {
			// Second hit, OK to leave first plane?
			if (distance(hitPlanes[0], dst) >= 0) {
				hitPlanes[0] = hitPlanes[1];
				break;
			}
			// No, slide along crease, intersection of 2 planes.
			auto crease = normalize(cross(hitPlanes[0].n, hitPlanes[1].n));
			dst = src + crease * dot(dst - src, crease);
			i = 2;
			break;
		}
		case 2: {
			// Third hit, Ok to leave crease planes?
			if (distance(hitPlanes[0], dst) >= 0 && distance(hitPlanes[1], dst) >= 0) {
				hitPlanes[0] = hitPlanes[2];
				i = 1;
				break;
			}
			// No, stop at point, intersection of 3 planes.
			return src;
		}
		default:
			SGD_ABORT();
		}
		// Never move backwards from original dir
		if (dot(dst - src, dir) <= 0) return src;
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

	auto wCoords = windowCoords / Vec2f(camera->scene()->viewportSize()) * 2.0f - 1.0f;

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
