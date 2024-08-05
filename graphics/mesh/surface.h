#pragma once

#include "../material/material.h"

namespace sgd {

struct Triangle {
	uint32_t indices[3];

	Triangle() = default;

	Triangle(uint32_t i0, uint32_t i1, uint32_t i2) : indices{i0, i1, i2} {
	}
};
using CTriangle = const Triangle&;

SGD_SHARED(Mesh);
SGD_SHARED(Surface);

struct Surface : Shared {
	SGD_OBJECT_TYPE(Surface, Shared);

	Surface(CMesh* mesh, Material* material, uint32_t triangleCount);

	CMesh* mesh()const{
		return m_mesh;
	}

	Material* material() const {
		return m_material;
	}

	const Triangle* triangles() const {
		return (const Triangle*)m_triangleBuffer->data();
	}

	uint32_t triangleCount() const {
		return m_triangleCount;
	}

	void resizeTriangles(uint32_t count);

	Triangle* lockTriangles(uint32_t first, uint32_t count) const {
		return (Triangle*)m_triangleBuffer->lock(first * sizeof(Triangle), count * sizeof(Triangle));
	}

	Triangle* lockTriangles() const {
		return lockTriangles(0, m_triangleCount);
	}

	void unlockTriangles() const {
		m_triangleBuffer->unlock();
	}

	void updateBounds();

	Buffer* triangleBuffer() const {
		return m_triangleBuffer;
	}

	Boxf bounds()const{
		return m_bounds;
	}

	Vec3f origin()const{
		return m_origin;
	}

private:
	CMesh* m_mesh;
	MaterialPtr m_material;
	uint32_t m_triangleCount;
	BufferPtr m_triangleBuffer;
	Boxf m_bounds;
	Vec3f m_origin;
};

}
