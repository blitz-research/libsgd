#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(MeshColliderData);

SGD_SHARED(MeshCollider);

struct MeshCollider : Collider {
	SGD_OBJECT_TYPE(MeshCollider, Collider);

	MeshCollider(Entity* entity, uint32_t colliderType, MeshColliderData* data);

	Collider* intersectRay(CLiner ray, float rradius, Contact& contact) override;

	Collider* intersectRay(CLiner ray, CVec3f radii, Contact& contact) override;

private:
	MeshColliderDataPtr m_data;

	void onUpdate(const CollisionSpace* space, uint32_t colliderMask, CollisionResponse response, Vector<Collision>& collisions) override;
};


}
