#include "bvhtree.h"

namespace sgd {

namespace {

int n_collide;

}

// ***** BVHNode *****

struct BVHNode {

	using LeafIterator = Vector<BVHNode*>::iterator;

	Collider* collider;
	Boxr bounds;
	BVHNode* parent;
	BVHNode* children[2]{}; // fully populated - both either null or non-null;

	BVHNode(Collider* collider, CBoxr bounds, BVHNode* parent) : collider(collider), bounds(bounds), parent(parent) {
	}

	~BVHNode() = default;

	void deleteAll() {	// NOLINT (recursive)
		if (children[0]) children[0]->deleteAll();
		if (children[1]) children[1]->deleteAll();
		delete this;
	}

	void validate() {	// NOLINT (recursive)
		if (parent) {
			SGD_ASSERT(parent->children[0] != nullptr && parent->children[1] != nullptr);
			SGD_ASSERT(parent->children[0] == this || parent->children[1] == this);
			SGD_ASSERT(parent->children[0] != this || parent->children[1] != this);
		}
		if (isLeaf()) {
			SGD_ASSERT(collider != nullptr && children[0] == nullptr && children[1] == nullptr);
		} else {
			SGD_ASSERT(children[0] != nullptr && children[1] != nullptr);
			SGD_ASSERT(children[0]->parent == this && children[1]->parent == this);
		}
	}

	void validateAll() {
		validate();
		if (children[0]) children[0]->validate();
		if (children[1]) children[1]->validate();
	}

	int getDepth() const {
		int depth = 0;
		for (BVHNode* p = parent; p; p = p->parent) ++depth;
		return depth;
	}

	bool isLeaf() const {
		return collider != nullptr && children[0] == nullptr && children[1] == nullptr;
	}

	BVHNode* sibling() const {
		return parent->children[parent->children[0] == this];
	}

	void insert(BVHNode* node) {

		int depth = 0;
		while (!node->isLeaf()) {
			node->bounds |= bounds;
			if (contains(node->children[0]->bounds,bounds)) {
				node = node->children[0];
			} else if (contains(node->children[1]->bounds,bounds)) {
				node = node->children[1];
			} else {
				auto grow0 = volume(node->children[0]->bounds | bounds) - volume(node->children[0]->bounds);
				auto grow1 = volume(node->children[1]->bounds | bounds) - volume(node->children[1]->bounds);
				node = node->children[grow1 < grow0];
			}
			++depth;
		}

		// debug() << "### insert @" << depth;

		BVHNode* gparent = node->parent;
		parent = new BVHNode(nullptr, node->bounds | bounds, gparent);
		if (gparent) gparent->children[gparent->children[1] == node] = parent;
		parent->children[0] = node;
		parent->children[1] = this;
		node->parent = parent;
	}

	void update() {

		// remove this
		BVHNode* gparent = parent->parent;
		BVHNode* sibling = this->sibling();
		if ((sibling->parent = gparent)) gparent->children[gparent->children[1] == parent] = sibling;

		// recalc parent bounds
		BVHNode* node = sibling;
		while (node->parent) {
			node = node->parent;
			auto newBounds = node->children[0]->bounds | node->children[1]->bounds;
			if (node->bounds == newBounds) break;
			node->bounds = newBounds;
		}

		// insert
		while (!node->isLeaf()) {
			node->bounds |= bounds;
			if (contains(node->children[0]->bounds,bounds)) {
				node = node->children[0];
			} else if (contains(node->children[1]->bounds,bounds)) {
				node = node->children[1];
			} else {
				auto grow0 = volume(node->children[0]->bounds | bounds) - volume(node->children[0]->bounds);
				auto grow1 = volume(node->children[1]->bounds | bounds) - volume(node->children[1]->bounds);
				node = node->children[grow1 < grow0];
			}
		}
		gparent = node->parent;
		parent->bounds = node->bounds | bounds;
		if ((parent->parent = gparent)) gparent->children[gparent->children[1] == node] = parent;
		parent->children[0] = node;
		parent->children[1] = this;
		node->parent = parent;
	}

	void remove() {
		SGD_ASSERT(isLeaf() && parent);

		// remove this
		BVHNode* gparent = parent->parent;
		BVHNode* sibling = this->sibling();
		if ((sibling->parent = gparent)) gparent->children[gparent->children[1] == parent] = sibling;
		delete parent;

		// recalc parent bounds
		BVHNode* node = sibling;
		while (node->parent) {
			node = node->parent;
			auto newBounds = node->children[0]->bounds | node->children[1]->bounds;
			if (node->bounds == newBounds) break;
			node->bounds = newBounds;
		}
	}

	Collider* intersectRay(const Liner& ray, CBoxr rayBounds, CVec3r radii, uint32_t colliderMask, Contact& contact) const { // NOLINT (recursive)

		if(!intersects(rayBounds, bounds)) return nullptr;

		if(!isLeaf()) {
			auto collider0 = children[0]->intersectRay(ray, rayBounds, radii, colliderMask, contact);
			auto collider1 = children[1]->intersectRay(ray, rayBounds, radii, colliderMask, contact);
			return collider1 ? collider1 : collider0;
		}

		if(!((1u << collider->colliderType()) & colliderMask)) return nullptr;

		return collider->intersectRay(ray, radii, contact);
	}

	void insertLeaves(LeafIterator begin, LeafIterator end) {	// NOLINT (recursive)

		for (auto it = begin; it != end; ++it) bounds |= (*it)->bounds;

		auto ext = size(bounds);

		if (ext.x > ext.y && ext.x > ext.z) {
			// sort leaves by x
			std::sort(begin, end, [](BVHNode* x, BVHNode* y) { return x->bounds.min.x < y->bounds.min.x; });
		} else if (ext.y > ext.z ){
			// sort leaves by y
			std::sort(begin, end, [](BVHNode* x, BVHNode* y) { return x->bounds.min.y < y->bounds.min.y; });
		} else {
			// sort leaves by z
			std::sort(begin, end, [](BVHNode* x, BVHNode* y) { return x->bounds.min.z < y->bounds.min.z; });
		}

		auto mid = begin + (end - begin) / 2;

		if (mid - begin == 1) {
			//		debug() << "### insert@" << getDepth() + 1;
			children[0] = *begin;
			children[0]->parent = this;
		} else {
			children[0] = new BVHNode(nullptr, {}, this);
			children[0]->insertLeaves(begin, mid);
		}
		if (end - mid == 1) {
			//		debug() << "### insert@" << getDepth() + 1;
			children[1] = *mid;
			children[1]->parent = this;
		} else {
			children[1] = new BVHNode(nullptr, {}, this);
			children[1]->insertLeaves(mid, end);
		}
	}

	void removeLeaves(Vector<BVHNode*>& leaves) {	// NOLINT (recursive)
		if (isLeaf()) {
			leaves.push_back(this);
			parent->children[parent->children[1] == this] = nullptr;
			parent = nullptr;
			return;
		}
		children[0]->removeLeaves(leaves);
		children[1]->removeLeaves(leaves);
	}
};

// ***** BVHTree *****

BVHTree::BVHTree() {
	clear();
}

BVHTree::~BVHTree() {
	if (m_root) m_root->deleteAll();
}

void BVHTree::clear() {
	if (m_root) m_root->deleteAll();
	m_root = nullptr;
}

BVHNode* BVHTree::insert(Collider* collider) {

	auto node = new BVHNode(collider, collider->worldBounds(), nullptr);

	//	debug() << "### Inserting node" << node->bounds;

	if (!m_root) {
		m_root = node;
		return node;
	}

	node->insert(m_root);
	while (m_root->parent) m_root = m_root->parent;

	m_root->validateAll();

	return node;
}

void BVHTree::update(BVHNode* node) {

	node->bounds = node->collider->worldBounds();

	if (node == m_root || contains(node->parent->bounds,node->bounds)) return;

	node->update();
	while (m_root->parent) m_root = m_root->parent;

	m_root->validateAll();
}

void BVHTree::remove(BVHNode* node) {

	if (node == m_root) {
		m_root = nullptr;
	} else if (node == m_root->children[0]) {
		BVHNode* root = m_root->children[1];
		delete m_root;
		m_root = root;
		m_root->parent = nullptr;
	} else if (node == m_root->children[1]) {
		BVHNode* root = m_root->children[0];
		delete m_root;
		m_root = root;
		m_root->parent = nullptr;
	} else {
		node->remove();
	}

	if (m_root) m_root->validateAll();

	delete node;
}

void BVHTree::optimize() {
	if (m_root->isLeaf()) return;

	Vector<BVHNode*> leaves;

	m_root->removeLeaves(leaves);

	m_root->deleteAll();
	m_root = new BVHNode(nullptr, {}, nullptr);
	m_root->insertLeaves(leaves.begin(), leaves.end());
}

Collider* BVHTree::intersectRay(const Liner& ray, CVec3r radii, uint32_t colliderMask, Contact& contact) const {

	if(!m_root) return nullptr;

	Boxr rayBounds(ray.o);
	rayBounds |= ray * contact.time;
	rayBounds.min -= radii;
	rayBounds.max += radii;

	return m_root->intersectRay(ray, rayBounds, radii, colliderMask, contact);
}

} // namespace sgd
