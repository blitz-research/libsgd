#pragma once

#include "renderpipeline.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Renderer);

struct RenderOp {
	wgpu::Buffer vertexBuffer;
	wgpu::Buffer indexBuffer;
	wgpu::Buffer instanceBuffer;
	wgpu::BindGroup materialBindGroup;
	wgpu::BindGroup rendererBindGroup;
	wgpu::RenderPipeline renderPipeline;
	uint32_t elementCount{};
	uint32_t instanceCount{};
	uint32_t firstElement{};
};

struct Renderer : GraphicsResource {

	Property<bool> enabled{true};

	virtual void onUpdate(CVec3f eye) {
	}

	void render(GraphicsContext* gc) const {
		if (!(1 << (int)gc->renderPass() & m_renderPassMask)) return;
		onRender(gc);
	}

protected:
	Vector<RenderOp> m_renderOps[4];

	mutable int m_renderPassMask{~0};

	Renderer() = default;

	virtual void onRender(GraphicsContext* gc) const = 0;
};

} // namespace sgd
