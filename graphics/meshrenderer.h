#pragma once

#include "mesh.h"
#include "renderer.h"

#include "shaders/uniforms.h"

namespace sgd {

// @location(8) matrix_0: vec4f,
// @location(9) matrix_1: vec4f,
// @location(10) matrix_2: vec4f,
// @location(11) matrix_3: vec4f,
// @location(12) color: vec4f,
struct MeshInstance {
	Mat4f matrix;
	Vec4f color;
};

SGD_SHARED(MeshRenderer);

struct MeshRenderer : Renderer {
	SGD_OBJECT_TYPE(MeshRenderer, Renderer);

	explicit MeshRenderer(CMesh* mesh);

	MeshInstance* lockInstances(uint32_t numInstances);
	void unlockInstances();

private:
	CMeshPtr m_mesh;
	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCapacity;
	BufferPtr m_instanceBuffer;

	uint32_t m_instanceCount{};

	mutable bool m_rebuildRenderOps{true};
	mutable bool m_updateInstanceCounts{true};

	void onValidate(GraphicsContext* gc) const override;
};

} // namespace sgd
