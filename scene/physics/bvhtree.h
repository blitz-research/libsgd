#pragma once

#include "collider.h"

namespace sgd {

struct BVHNode;

SGD_SHARED(BVHTree);

struct BVHTree : public Shared {
	SGD_OBJECT_TYPE(BVHTree, Shared);

	static constexpr real boundsPadding = (real).01;

	BVHTree();
	~BVHTree() override;

	void clear();

	BVHNode* insert(Collider* collider);

	void update(BVHNode* node);

	void remove(BVHNode* node);

	void optimize();

	Collider* intersectRay(const Liner& ray, CVec3r radii, uint32_t colliderMask, Contact& contact) const;

private:
	BVHNode* m_root = nullptr;
};

} // namespace wb
