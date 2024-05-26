#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(Collider);
SGD_SHARED(CollisionSpace);

struct CollisionNode : Object {
	SGD_OBJECT_TYPE(CollisionNode, Object);

	CollisionNode(Collider* collider, CollisionSpace* space);

	Collider* collider() const {
		return m_collider;
	}

	CollisionSpace* space() const {
		return m_space;
	}

private:
	friend class CollisionSpace;

	ColliderPtr m_collider;
	CollisionSpace* m_space;
};

struct CollisionSpace : Shared {
	SGD_OBJECT_TYPE(CollisionSpace, Shared);

	void enableCollisions(uint32_t srcColliderType, uint32_t dstColliderType, CollisionResponse response);

	CollisionNode* insert(Collider* collider);

	void remove(CollisionNode* node);

	Collider* intersectRay(const Liner& ray, real radius, uint32_t colliderMask, Contact& contact) const;
	Collider* intersectRay(const Liner& ray, CVec3r radii, uint32_t colliderMask, Contact& contact) const;

	void updateColliders();

private:
	friend class CollisionNode;

	struct EnabledCollision {
		uint32_t srcType;
		uint32_t dstType;
		CollisionResponse response;
	};

	Vector<EnabledCollision> m_enabledCollisions[32];
	uint32_t m_colliderMasks[32];

	Vector<CollisionNode*> m_collisionNodes[32];
};

} // namespace sgd
