#pragma once

#include "collider.h"

#include "contact.h"

namespace sgd {

SGD_SHARED(Mesh);

SGD_SHARED(MeshColliderData);

struct MeshColliderData : Shared {
	SGD_OBJECT_TYPE(MeshColliderData, Shared);

	static constexpr real boundsPadding = (real).01;

	explicit MeshColliderData(CMesh* mesh);

	bool intersectRay(CLiner ray, float radius, Contact& contact) const;

	bool intersectRay(CLiner ray, CVec3f radii, Contact& contact) const;

	Boxr bounds() const {
		return m_bounds;
	}

private:
	struct Node;

	struct Triangle {
		uint32_t indices[3];
	};

	Vector<Vec3f> m_vertices;
	Node* m_rootNode{};
	Boxr m_bounds;

	Node* createNode(Vector<Triangle> triangles);
};

MeshColliderData* getOrCreateMeshColliderData(CMesh* mesh);

} // namespace sgd
