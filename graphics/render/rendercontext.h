#pragma once

#include "../core/bindgroup.h"
#include "renderop.h"
#include "renderpipeline.h"

namespace sgd {

SGD_SHARED(RenderContext);

struct RenderContext : Shared {
	SGD_OBJECT_TYPE(RenderContext, Shared);

	wgpu::CommandEncoder beginRender();

	wgpu::RenderPassEncoder beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
											float clearDepth, const wgpu::BindGroup& sceneBindings);

	RenderPassType renderPassType() const {
		return m_renderPassType;
	}

	void render(CVector<RenderOp>& ops);

	void endRenderPass();

	void endRender();

#if 0
	const wgpu::CommandEncoder& wgpuCommandEncoder() const {
		return m_wgpuCommandEncoder;
	}

	const wgpu::RenderPassEncoder& wgpuRenderPassEncoder() const {
		return m_wgpuRenderPassEncoder;
	}
#endif

private:
	enum struct Dirty { vbuffer = 1, ibuffer = 2, material = 4, geometry = 8, renderer = 16 };

	wgpu::CommandEncoder m_wgpuCommandEncoder;
	wgpu::RenderPassEncoder m_wgpuRenderPassEncoder;

	RenderPassType m_renderPassType{};

	RenderOp m_rop;
};

} // namespace sgd
