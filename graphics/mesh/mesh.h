#pragma once

#include "surface.h"
#include "vertex.h"

namespace sgd {

struct alignas(16) MeshUniforms {
	uint32_t tangentsEnabled{0};
};

enum struct MeshFlags {
	none = 0,
	tangentsEnabled = 1,
	blendedSurfaces = 2,
};

SGD_SHARED(Mesh);

struct Mesh : GraphicsResource {
	SGD_OBJECT_TYPE(Mesh, GraphicsResource);

	Mesh(uint32_t vertexCount, MeshFlags flags);

	Property<bool> shadowsEnabled;

	MeshFlags flags() const {
		return m_flags;
	}

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

	void clearSurfaces();

	void addSurface(Surface* surface);

	void updateBounds();

	CVector<SurfacePtr> surfaces() const {
		return m_surfaces;
	}

	CSurface* surface(uint32_t index) const {
		return m_surfaces[index];
	}

	Surface* surface(uint32_t index) {
		return m_surfaces[index];
	}

	Buffer* vertexBuffer() const {
		return m_vertexBuffer;
	}

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

	Boxf bounds()const {
		return m_bounds;
	}

	Vec3f origin()const {
		return m_origin;
	}

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_vertexBuffer;
	uint32_t m_vertexCount;
	MeshFlags m_flags;
	Vector<SurfacePtr> m_surfaces;

	Boxf m_bounds;
	Vec3f m_origin;
};

} // namespace sgd
