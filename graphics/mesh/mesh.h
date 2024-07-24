#pragma once

#include "../core/bindgroup.h"
#include "../material/material.h"

namespace sgd {

struct alignas(16) MeshUniforms {
	int32_t tangentsEnabled{0};
};

struct Vertex {
	Vec3f position;
	Vec3f normal;
	Vec4f tangent;
	Vec3f texCoords;
	Vec4f color{1};
	uint8_t joints[4]{};
	float weights[4]{};

	Vertex() = default;

	Vertex(CVec3f position, CVec3f normal, CVec2f texCoords) : position(position), normal(normal), texCoords(texCoords, 0) {
	}

	Vertex(CVec3f position, CVec2f texCoords, CVec4f color) : position(position), texCoords(texCoords, 0), color(color) {
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

SGD_SHARED(Surface);

struct Surface : Shared {
	SGD_OBJECT_TYPE(Surface, Shared);

	Surface(uint32_t triangleCount, Material* material);

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

	Buffer* triangleBuffer() const {
		return m_triangleBuffer;
	}

	Material* material() const {
		return m_material;
	}

private:
	BufferPtr m_triangleBuffer;
	uint32_t m_triangleCount;
	MaterialPtr m_material;
};

SGD_SHARED(Mesh);

enum struct MeshFlags { none = 0, tangentsEnabled = 1 };

struct Mesh : GraphicsResource {
	SGD_OBJECT_TYPE(Mesh, GraphicsResource);

	Mesh(uint32_t vertexCount, MeshFlags flags);

	Property<bool> shadowsEnabled;

	const Vertex* vertices() const {
		return (Vertex*)m_vertexBuffer->data();
	}

	uint32_t vertexCount() const {
		return m_vertexCount;
	}

	void resizeVertices(uint32_t count);

	Vertex* lockVertices(uint32_t first, uint32_t count) const {
		return (Vertex*)m_vertexBuffer->lock(first * sizeof(Vertex), count * sizeof(Vertex));
	}

	Vertex* lockVertices() const {
		return lockVertices(0, m_vertexCount);
	}

	void unlockVertices() const {
		m_vertexBuffer->unlock();
	}

	Buffer* vertexBuffer() const {
		SGD_ASSERT(m_vertexBuffer);
		return m_vertexBuffer;
	}

	void clearSurfaces();

	void addSurface(Surface* surface);

	CVector<SurfacePtr> surfaces() const {
		return m_surfaces;
	}

	CSurface* surface(uint32_t index) const {
		return m_surfaces[index];
	}

	Surface* surface(uint32_t index) {
		return m_surfaces[index];
	}

	MeshFlags flags() const {
		return m_flags;
	}

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_vertexBuffer;
	uint32_t m_vertexCount;
	MeshFlags m_flags;
	Vector<SurfacePtr> m_surfaces;

};

} // namespace sgd
