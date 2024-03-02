#pragma once

#include "buffer.h"
#include "material.h"

namespace sgd {

struct Vertex {
	Vec3f position;
	Vec3f normal;
	Vec4f tangent;
	Vec2f texCoords;
	Vec4f color{1};

	Vertex() = default;

	Vertex(CVec3f position, CVec3f normal, CVec2f texCoords) : position(position), normal(normal), texCoords(texCoords) {
	}

	Vertex(CVec3f position, CVec2f texCoords) : position(position), texCoords(texCoords) {
	}

	Vertex(CVec3f position, CVec3f normal) : position(position), normal(normal) {
	}
};
using CVertex = const Vertex&;

struct Triangle {
	uint32_t indices[3];

	Triangle() = default;

	Triangle(uint32_t i0, uint32_t i1, uint32_t i2) : indices{i0, i1, i2} {
	}
};
using CTriangle = const Triangle&;

struct Surface {
	MaterialPtr material;
	uint32_t firstTriangle;
	uint32_t triangleCount;
};
using CSurface = const Surface&;

SGD_SHARED(Mesh);

enum struct MeshFlags {
	none = 0,
	tangentsEnabled = 1

};

struct Mesh : GraphicsResource {
	SGD_OBJECT_TYPE(Mesh, GraphicsResource);

	Mesh(const Vertex* vertices, uint32_t vertexCount, const Triangle* triangles, uint32_t triangleCount, CVector<Surface> surfaces, MeshFlags flags);

	Mesh(CVector<Vertex> vertices, CVector<Triangle> triangles, CVector<Surface> surfaces, MeshFlags flags);

	MeshFlags flags()const{ return m_flags;}

	const Vertex* vertices() const {
		return (Vertex*)m_vertexBuffer->data();
	}

	uint32_t vertexCount() const {
		return m_vertexCount;
	}

	const Triangle* triangles() const {
		return (Triangle*)m_triangleBuffer->data();
	}

	uint32_t triangleCount() const {
		return m_triangleCount;
	}

	CVector<Surface> surfaces() const {
		return m_surfaces;
	}

	void resizeVertices(uint32_t size);
	void addVertices(const Vertex* data, uint32_t count);
	Vertex* lockVertices(uint32_t first, uint32_t count);
	void unlockVertices();

	void resizeTriangles(uint32_t size);
	void addTriangles(const Triangle* data, uint32_t count);
	Triangle* lockTriangles(uint32_t first, uint32_t count);
	void unlockTriangles();

	void clearSurfaces();
	void addSurface(CSurface surface);

	CBuffer* vertexBuffer() const {
		return m_vertexBuffer;
	}

	CBuffer* indexBuffer() const {
		return m_triangleBuffer;
	}

private:
	BufferPtr m_vertexBuffer;
	uint32_t m_vertexCount{};

	BufferPtr m_triangleBuffer;
	uint32_t m_triangleCount{};

	Vector<Surface> m_surfaces;

	MeshFlags m_flags;
};

} // namespace sgd
