#include "collisionutil.h"

#include "../scene/scene.h"

namespace sgd {

SGD_SHARED(Camera);

template <class IntersectFunc>
Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, IntersectFunc intersectFunc, uint32_t colliderMask,
				 uint32_t colliderType, Vector<Collision>& collisions) {

	static constexpr real mind = (real).0001;
	static constexpr real eps = (real).01;

	static constexpr int maxLoops = 6;

	auto dir = dst - src;
	auto dist = length(dir);
	if (dist <= mind) return src;

	dir /= dist;
	auto fwd = dir;

	Planer planes[3];
	int state = 0;
	int loops = 0;

	for (;;) {

		Contact contact(dist);
		Line ray(src, dir);

		auto collider = intersectFunc(ray, colliderMask, contact);
		if (!collider) return dst;

		collisions.emplace_back(ray, contact, collider);

		auto response = space->responseForColliderTypes(colliderType, collider->colliderType());

		if (response == CollisionResponse::ignore) {
			colliderMask &= ~(1 << collider->colliderType());
			continue;
		}

		auto hitd = std::max(contact.time - eps, (real)0); // stop before plane
		src = ray * hitd;

		if (response == CollisionResponse::stop) {
			return src;
		}

		// Avoid looping forever
		if (++loops == maxLoops) {
#if SGD_CONFIG_DEBUG
			SGD_LOG << "collideRay loops == maxLoops! ray:" << ray << "contact:" << contact;
#endif
			return src;
		}

		if ((dist -= hitd) <= mind) return src;
		auto dist_xz = length(Vec2r(dst.x - src.x, dst.z - src.z));

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

		if (d > dist) {
			SGD_LOG << "### Clip error, d:" << d << "dist:" << dist;
			dst = src + dir / d * dist;
			d = dist;
		}
		dir /= d;

		if (response == CollisionResponse::slidexz) {
			auto d_xz = length(Vec2r(dst.x - src.x, dst.z - src.z));
			if (d_xz > dist_xz) std::swap(d_xz, dist_xz);
			dist *= d_xz / dist_xz;
			if (dist <= mind) return src;
			dst = src + dir * dist;
			dist_xz = d_xz;
		} else {
			dist_xz *= d / dist;
			dist = d;
		}
	}
}

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, float radius, uint32_t colliderMask, uint32_t colliderType,
				 Vector<Collision>& collisions) {

	auto intersectFunc = [=](CLiner ray, uint32_t colliderMask, Contact& contact) -> Collider* {
		return space->intersectRay(ray, radius, colliderMask, contact);
	};

	return collideRay(space, src, dst, intersectFunc, colliderMask, colliderType, collisions);
}

Vec3r collideRay(const CollisionSpace* space, Vec3r src, Vec3r dst, CVec3f radii, uint32_t colliderMask, uint32_t colliderType,
				 Vector<Collision>& collisions) {

	auto intersectFunc = [=](CLiner ray, uint32_t colliderMask, Contact& contact) -> Collider* {
		return space->intersectRay(ray, radii, colliderMask, contact);
	};

	return collideRay(space, src, dst, intersectFunc, colliderMask, colliderType, collisions);
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
