#pragma once

#include "mesh.h"

namespace sgd {

struct alignas(16) MeshInstance {
	Mat4f worldMatrix;
	Vec4f color;
};

SGD_SHARED(RenderQueue);

SGD_SHARED(MeshRenderer);

struct MeshRenderer : Shared {
	SGD_OBJECT_TYPE(MeshRenderer, Shared);

	MeshRenderer();

	MeshInstance* lockInstances(uint32_t numInstances);
	void unlockInstances();

	void render(RenderQueue* rq, CMesh* mesh, uint32_t count, uint32_t first) const;

	void render(RenderQueue* rq, CSurface* surface, uint32_t count, uint32_t index) const;

	BindGroup* bindGroup()const {
		return m_bindGroup;
	}

private:
	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCapacity{1};
	BufferPtr m_instanceBuffer;
	uint32_t m_instanceCount{};
};

} // namespace sgd
