#pragma once

#include "mesh.h"
#include "renderer.h"

namespace sgd {

// @group(2) @binding(1) var<storage> bonedMeshInstances: array<SkinnedMeshInstance>;
struct alignas(16) SkinnedMeshInstance {
	static constexpr int maxJoints=256;
	Mat4f worldMatrix;
	Vec4f color;
	Mat4f jointMatrices[maxJoints];
};

SGD_SHARED(SkinnedMeshRenderer);

struct SkinnedMeshRenderer : Renderer{
	SGD_OBJECT_TYPE(SkinnedMeshRenderer, Renderer);

	explicit SkinnedMeshRenderer(CMesh* mesh);

	SkinnedMeshInstance* lockInstances(uint32_t numInstances);
	void unlockInstances();

private:
	CMeshPtr m_mesh;
	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCount;

	mutable BufferPtr m_instanceBuffer;
	mutable uint32_t m_instanceCapacity{0};

	mutable wgpu::Buffer m_vertexBuffer;
	mutable wgpu::Buffer m_indexBuffer;
	mutable wgpu::BindGroup m_bindGroup2;

	struct RenderOp {
		wgpu::RenderPipeline pipeline;
		wgpu::BindGroup bindGroup1;
		uint32_t firstIndex;
		uint32_t indexCount;
	};

	mutable Vector<RenderOp> m_renderOps[renderPassCount];

	void onValidate(GraphicsContext* gc) const override;

	void onRender(GraphicsContext* gc) const override;
};

}
