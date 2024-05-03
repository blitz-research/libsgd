#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(SphereCollider);

struct SphereCollider : Collider {
	SGD_OBJECT_TYPE(SphereCollider, Collider);

	explicit SphereCollider(Entity* entity, uint32_t collisionType, uint32_t collisionMask, float iradius);

	Property<real> radius;

	const Collider* intersectRay(CLiner ray, real rradius, Contact& contact) const override;
};

}
