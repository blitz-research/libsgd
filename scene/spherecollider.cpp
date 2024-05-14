#include "spherecollider.h"

#include "collisionspace.h"
#include "collisionutil.h"
#include "intersect.h"

namespace sgd {

SphereCollider::SphereCollider(Entity* entity, uint32_t colliderType, float iradius)
	: Collider(entity, colliderType), radius(iradius), m_src(entity->worldPosition()) {

	this->radius.changed.connect(nullptr, [=](float r) { setLocalBounds({-r, r}); });
}

Collider* SphereCollider::intersectRay(CLiner ray, real rradius, Contact& contact) {

	if (!intersectRaySphere(ray, entity()->worldMatrix().t, rradius + radius(), contact)) return nullptr;

	contact.point -= contact.normal * rradius;

	return this;
}

void SphereCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask, CollisionResponse response, Vector<Collision>& collisions) {
	auto dst = entity()->worldPosition();

	dst = collideRay(space, m_src, dst, radius(), colliderMask, response, collisions);

	entity()->setWorldPosition(dst);
	m_src = dst;
}

void SphereCollider::onReset(Entity* entity) {

	m_src = entity->worldPosition();
}

} // namespace sgd
