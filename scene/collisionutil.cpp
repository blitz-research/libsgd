#include "collisionutil.h"

#include "camera.h"
#include "scene.h"

namespace sgd {

SGD_SHARED(Camera);

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, real radius, uint32_t colliderMask,
				 CollisionResponse response, Vector<Collision>& collisions) {

	static constexpr real eps = (real).0001;

	// Max distance to travel
	auto dir = dst - src;
	auto d = length(dir);
	if (d <= eps) return src;

	Planer hitPlanes[3];
	int n = 0;
	int i = 0;

	for (;;) {

		Contact contact(d);

		Line ray(src, normalize(dst - src));

		auto collider = space->intersectRay(ray, radius, colliderMask, contact);
		if (!collider) return dst;

		SGD_ASSERT(contact.time >= 0);
		++n;

		collisions.emplace_back(ray, contact, collider);

		if (response == CollisionResponse::ignore) return dst;

		auto t = std::max(contact.time - eps, (real)0);
		src = ray * t;

		if (response == CollisionResponse::stop) return src;

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

Collider* intersectRay(Camera* camera, CVec2f windowCoords, int colliderMask, Contact& rcontact) {

	auto wCoords = windowCoords / Vec2f(camera->scene()->viewportSize()) * 2.0f - 1.0f;

	auto clipCoords = camera->inverseProjectionMatrix() * Vec4f(wCoords.x, -wCoords.y, 0, 1);

	auto viewPos = clipCoords.xyz() / clipCoords.w;

	auto r = camera->far() - camera->near();
	auto d = std::sqrt(viewPos.x * viewPos.x + viewPos.y * viewPos.y + r * r);
	Contact contact(d);

	auto worldPos = camera->worldMatrix() * Vec3r(viewPos);

	Liner ray(worldPos, normalize(worldPos - camera->worldPosition()));

	auto collider = camera->scene()->collisionSpace()->intersectRay(ray, 0, colliderMask, contact);
	if (collider) rcontact = contact;

	return collider;
}

} // namespace sgd
