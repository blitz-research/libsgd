#pragma once

#include "collider.h"

namespace sgd {

SGD_SHARED(Collider);
SGD_SHARED(CollisionSpace);

struct CollisionNode : Object {
	SGD_OBJECT_TYPE(CollisionNode, Object);

	CollisionNode(Collider* collider, CollisionSpace* space);

	const Collider* collider() const {
		return m_collider;
	}

	CollisionSpace* space() const {
		return m_space;
	}

	void invalidate();

private:
	friend class CollisionSpace;

	ColliderPtr m_collider;
	CollisionSpace* m_space;
	bool m_invalid{};
};

struct CollisionSpace : Shared {
	SGD_OBJECT_TYPE(CollisionSpace, Shared);

	CollisionNode* insert(Collider* collider);

	void remove(CollisionNode* node);

	void validate();

	const Collider* intersectRay(const Liner& ray, real rradius, uint32_t collisionMask, Contact& contact);

private:
	friend class CollisionNode;

	Vector<CollisionNode*> m_nodes[32];
	Vector<CollisionNode*> m_invalid;
};

} // namespace sgd
