#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(MeshColliderData);

SGD_SHARED(MeshCollider);

struct MeshCollider : Collider {
	SGD_OBJECT_TYPE(MeshCollider, Collider);

	MeshCollider(Entity* entity, uint32_t collisionType, uint32_t collisionMask, MeshColliderData* data);

	const Collider* intersectRay(CLiner ray, real rradius, Contact& contact) const override;

private:
	MeshColliderDataPtr m_data;
};


}
