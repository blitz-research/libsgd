#include "ellipsoidcollider.h"

#include "collisionspace.h"
#include "collisionutil.h"
#include "intersect.h"

namespace sgd {

EllipsoidCollider::EllipsoidCollider(Entity* entity, uint32_t colliderType, float rradius, float hheight)
	: Collider(colliderType), radius(rradius), height(hheight), m_src(entity->worldPosition()) {

	auto updateBounds = [=] { setLocalBounds({{-radius(), -height() / 2, -radius()}, {radius(), height() / 2, radius()}}); };

	this->radius.changed.connect(nullptr, [=](float) { updateBounds(); });

	this->height.changed.connect(nullptr, [=](float) { updateBounds(); });

	updateBounds();

	attach(entity);
}

Collider* EllipsoidCollider::intersectRay(CLiner ray, float rradius, Contact& contact) {
	SGD_ERROR("TODO");
}

Collider* EllipsoidCollider::intersectRay(CLiner ray, CVec3f radii, Contact& contact) {
	SGD_ERROR("TODO");
}

void EllipsoidCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) {

	auto dst = collideRay(space, m_src, entity()->worldPosition(), radii(), colliderMask, colliderType(), collisions);

	entity()->setWorldPosition(dst);

	m_src = dst;

	for (auto& c : collisions) c.contact.point -= c.contact.normal * Vec3r(radii());
}

void EllipsoidCollider::onReset() {

	m_src = entity()->worldPosition();
}

} // namespace sgd
