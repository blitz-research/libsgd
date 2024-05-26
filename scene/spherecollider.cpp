#include "spherecollider.h"

#include "collisionspace.h"
#include "collisionutil.h"
#include "intersect.h"

namespace sgd {

SphereCollider::SphereCollider(Entity* entity, uint32_t colliderType, float rradius)
	: Collider(entity, colliderType), radius(rradius), m_src(entity->worldPosition()) {

	this->radius.changed.connect(nullptr, [=](float r) { setLocalBounds({-r, r}); });

	setLocalBounds({-rradius, rradius});
}

Collider* SphereCollider::intersectRay(CLiner ray, float rradius, Contact& contact) {

	if (!intersectRaySphere(ray, entity()->worldMatrix().t, rradius + radius(), contact)) return nullptr;

	contact.point -= contact.normal * (real)rradius;

	return this;
}

Collider* SphereCollider::intersectRay(CLiner ray, CVec3f radii, Contact& contact) {
	SGD_PANIC("TODO");

	return nullptr;
}

void SphereCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask, CollisionResponse response,
							  Vector<Collision>& collisions) {

	auto dst = collideRay(space, m_src, entity()->worldPosition(), radius(), colliderMask, response, collisions);

	entity()->setWorldPosition(dst);

	m_src = dst;
}

void SphereCollider::onReset(Entity* entity) {

	m_src = entity->worldPosition();
}

} // namespace sgd
