#include "planecollider.h"

#include "collisionspace.h"
#include "collisionutil.h"
#include "intersect.h"

namespace sgd {

PlaneCollider::PlaneCollider(Entity* entity, uint32_t colliderType, CPlanef plane) : Collider(colliderType), m_plane(plane) {

	float extent = 1 << 20;

	setLocalBounds({{-extent, 0, -extent}, {extent, 0, extent}});

	attach(entity);
}

Collider* PlaneCollider::intersectRay(CLiner ray, float rradius, Contact& contact) {

	auto plane = AffineMat4r(entity()->worldBasis(), entity()->worldPosition()) * Planer(m_plane);
	plane.d -= rradius;

	auto t = t_intersect(plane, ray);
	if (t < 0 || t >= contact.time) return nullptr;

	contact.point = ray * t - plane.n * (real)rradius;
	contact.normal = plane.n;
	contact.time = t;

	return this;
}

Collider* PlaneCollider::intersectRay(CLiner ray, CVec3f radii, Contact& contact) {
	SGD_ERROR("TODO");
	return nullptr;
}

void PlaneCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) {
}

} // namespace sgd
