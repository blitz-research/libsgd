#include "collisionspace.h"

namespace sgd {

// ***** CollisionNode *****

CollisionNode::CollisionNode(Collider* collider, CollisionSpace* space) : m_collider(collider), m_space(space) {
}

// ***** CollisionSpace *****

void CollisionSpace::enableCollisions(uint32_t srcType, uint32_t dstType, CollisionResponse response) {

	auto& info = m_colliderInfos[srcType];

	info.mask |= 1 << dstType;
	info.responses[dstType] = response;
}

CollisionNode* CollisionSpace::insert(Collider* collider) {
	auto& info = m_colliderInfos[collider->colliderType()];

	auto node = new CollisionNode(collider, this);
	info.nodes.push_back(node);

	return node;
}

void CollisionSpace::remove(CollisionNode* node) {
	auto& info = m_colliderInfos[node->collider()->colliderType()];

	sgd::remove(info.nodes, node);
}

Collider* CollisionSpace::intersectRay(const Liner& ray, real radius, uint32_t colliderMask, Contact& contact) const {
	Collider* collider{};
	for (uint32_t i = 0; i < 32; ++i) {
		if (!((1 << i) & colliderMask)) continue;
		for (auto node : m_colliderInfos[i].nodes) {
			auto r = node->collider()->intersectRay(ray, (float)radius, contact);
			if (r) collider = r;
		}
	}
	return collider;
}

Collider* CollisionSpace::intersectRay(const Liner& ray, CVec3r radii, uint32_t colliderMask, Contact& contact) const {
	Collider* collider{};
	for (uint32_t i = 0; i < 32; ++i) {
		if (!((1 << i) & colliderMask)) continue;
		for (auto node : m_colliderInfos[i].nodes) {
			auto r = node->collider()->intersectRay(ray, radii, contact);
			if (r) collider = r;
		}
	}
	return collider;
}

void CollisionSpace::updateColliders() {
	for(auto& info : m_colliderInfos) {
		if(!info.mask) continue;
		for (auto node : info.nodes) {
			auto collider = node->collider();
			collider->update(info.mask);
		}
	}
}

} // namespace sgd
