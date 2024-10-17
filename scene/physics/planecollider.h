#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(PlaneCollider);

struct PlaneCollider : Collider {
	SGD_OBJECT_TYPE(PlaneCollider, Collider);

	explicit PlaneCollider(Entity* entity, uint32_t colliderType, CPlanef plane);

	Property<float> radius;

	Collider* intersectRay(CLiner ray, float rradius, Contact& contact) override;

	Collider* intersectRay(CLiner ray, CVec3f radii, Contact& contact) override;

private:
	Planef m_plane;

	void onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) override;
};

} // namespace sgd
