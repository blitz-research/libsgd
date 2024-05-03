#include "collisionspace.h"

namespace sgd {

// ***** CollisionNode *****

CollisionNode::CollisionNode(Collider* collider, CollisionSpace* space) : m_collider(collider), m_space(space) {
}

void CollisionNode::invalidate() {
	if (m_invalid) return;
	m_invalid = true;
	m_space->m_invalid.push_back(this);
}

// ***** CollisionSpace *****

CollisionNode* CollisionSpace::insert(Collider* collider) {

	SGD_LOG << "inserting collider" << collider << "collisionType" << collider->collisionType();

	auto node = new CollisionNode(collider, this);
	m_nodes[collider->collisionType()].push_back(node);
	return node;
}

void CollisionSpace::remove(CollisionNode* node) {
	sgd::remove(m_nodes[node->collider()->collisionType()], node);
}

void CollisionSpace::validate() {
	for (auto node : m_invalid) node->m_invalid = false;
	m_invalid.clear();
}

const Collider* CollisionSpace::intersectRay(const Liner& ray, real rradius, uint32_t collisionMask, Contact& contact) {
	validate();

	const Collider* collider{};

	for (uint32_t i = 0; i < 32; ++i) {
		if (!((1 << i) & collisionMask)) continue;
		for (auto node : m_nodes[i]) {
			auto r = node->collider()->intersectRay(ray, rradius, contact);
			if (r) collider = r;
		}
	}

	return collider;
}

} // namespace sgd
