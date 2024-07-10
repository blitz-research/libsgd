#pragma once

#include "mesh.h"

namespace sgd {

struct alignas(16) MeshInstance {
	Mat4f worldMatrix;
	Vec4f color;
};

SGD_SHARED(RenderContext);

SGD_SHARED(MeshRenderer);

struct MeshRenderer : Shared {
	SGD_OBJECT_TYPE(MeshRenderer, Shared);

	explicit MeshRenderer(CMesh* mesh);

	MeshInstance* lockInstances(uint32_t numInstances);
	void unlockInstances();

	void render(RenderContext* rc) const;

	BindGroup* bindGroup()const {
		return m_bindGroup;
	}

private:
	CMeshPtr m_mesh;
	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCapacity;
	BufferPtr m_instanceBuffer;
	uint32_t m_instanceCount{};
};

} // namespace sgd
