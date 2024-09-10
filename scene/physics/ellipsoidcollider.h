#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(EllipsoidCollider);

struct EllipsoidCollider : Collider {
	SGD_OBJECT_TYPE(EllipsoidCollider, Collider);

	explicit EllipsoidCollider(Entity* entity, uint32_t colliderType, float radius, float height);

	Property<float> radius;

	Property<float> height;

	Vec3f radii() const {
		return {radius(), height() * .5f, radius()};
	}

	Collider* intersectRay(CLiner ray, float rradius, Contact& contact) override;

	Collider* intersectRay(CLiner ray, CVec3f radii, Contact& contact) override;

private:
	Vec3r m_src;

	void onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) override;

	void onReset() override;
};

} // namespace sgd
