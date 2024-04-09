#pragma once

#include "mesh.h"
#include "renderer.h"

namespace sgd {

struct SkinnedMeshInstance;

SGD_SHARED(SkinnedMeshRenderer);

struct SkinnedMeshRenderer : Renderer{
	SGD_OBJECT_TYPE(SkinnedMeshRenderer, Renderer);

	explicit SkinnedMeshRenderer(CMesh* mesh);

	SkinnedMeshInstance* lockInstances(uint32_t numInstances);
	void unlockInstances();

private:
	CMeshPtr m_mesh;
	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCount{};
	uint32_t m_instanceCapacity{10};
	BufferPtr m_instanceBuffer;

	mutable bool m_rebuildRenderOps{true};
	mutable bool m_updateInstanceCounts{true};

	void onValidate(GraphicsContext* gc) const override;
};

}
