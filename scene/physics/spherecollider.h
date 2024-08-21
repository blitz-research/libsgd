#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(SphereCollider);

struct SphereCollider : Collider {
	SGD_OBJECT_TYPE(SphereCollider, Collider);

	explicit SphereCollider(Entity* entity, uint32_t colliderType, float radius);

	Property<float> radius;

	Collider* intersectRay(CLiner ray, float rradius, Contact& contact) override;

	Collider* intersectRay(CLiner ray, CVec3f radii, Contact& contact) override;

private:
	Vec3r m_src;

	void onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) override;

	void onReset(Entity* entity) override;
};

} // namespace sgd
