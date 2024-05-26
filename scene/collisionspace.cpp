#include "collisionspace.h"

namespace sgd {

// ***** CollisionNode *****

CollisionNode::CollisionNode(Collider* collider, CollisionSpace* space) : m_collider(collider), m_space(space) {
}

// ***** CollisionSpace *****

void CollisionSpace::enableCollisions(uint32_t srcType, uint32_t dstType, CollisionResponse response) {

	if (m_colliderMasks[srcType] & (1 << dstType)) return;

	m_enabledCollisions[srcType].push_back({srcType, dstType, response});

	m_colliderMasks[srcType] |= (1 << dstType);
}

CollisionNode* CollisionSpace::insert(Collider* collider) {

	SGD_LOG << "inserting collider" << collider << "collisionType" << collider->colliderType();

	auto node = new CollisionNode(collider, this);
	m_collisionNodes[collider->colliderType()].push_back(node);

	return node;
}

void CollisionSpace::remove(CollisionNode* node) {

	sgd::remove(m_collisionNodes[node->collider()->colliderType()], node);
}

Collider* CollisionSpace::intersectRay(const Liner& ray, real radius, uint32_t colliderMask, Contact& contact) const {

	Collider* collider{};

	for (uint32_t i = 0; i < 32; ++i) {
		if (!((1 << i) & colliderMask)) continue;
		for (auto node : m_collisionNodes[i]) {
			auto r = node->collider()->intersectRay(ray, radius, contact);
			if (r) collider = r;
		}
	}

	return collider;
}

Collider* CollisionSpace::intersectRay(const Liner& ray, CVec3r radii, uint32_t colliderMask, Contact& contact) const {

	Collider* collider{};

	for (uint32_t i = 0; i < 32; ++i) {
		if (!((1 << i) & colliderMask)) continue;
		for (auto node : m_collisionNodes[i]) {
			auto r = node->collider()->intersectRay(ray, radii, contact);
			if (r) collider = r;
		}
	}

	return collider;
}

void CollisionSpace::updateColliders() {
	for (uint32_t i = 0; i < 32; ++i) {
		auto mask = m_colliderMasks[i];
		for (auto node : m_collisionNodes[i]) {
			auto collider = node->collider();
			for (auto& coll : m_enabledCollisions[i]) {
				collider->update(mask, coll.response);
			}
		}
	}
}

} // namespace sgd
