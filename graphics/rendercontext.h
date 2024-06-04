#pragma once

#include "graphics.h"
#include "renderop.h"
#include "renderpipeline.h"
#include "shaders/uniforms.h"

namespace sgd {

SGD_SHARED(RenderContext);

struct RenderContext : Shared {
	SGD_OBJECT_TYPE(RenderContext, Shared);

	Property<CameraUniforms> cameraUniforms;

	Property<LightingUniforms> lightingUniforms;

	Property<TexturePtr> envTexture;

	explicit RenderContext(GraphicsContext* gc);

	GraphicsContext* gc() const {
		return m_gc;
	}

	RenderPassType renderPassType() const {
		return m_renderPassType;
	}

	// Move clear color/depth to scene bindings?
	void beginRender();

	void beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor, float clearDepth);

	void render(CVector<RenderOp> ops);

	void endRenderPass();

	void endRender();

	const wgpu::Device& wgpuDevice() {
		return m_gc->wgpuDevice();
	}

	const wgpu::CommandEncoder& wgpuCommandEncoder() const {
		return m_wgpuCommandEncoder;
	}

	const wgpu::RenderPassEncoder& wgpuRenderPassEncoder() const {
		return m_wgpuRenderPassEncoder;
	}

private:
	GraphicsContextPtr m_gc;

	RenderOp m_rop;

	RenderPassType m_renderPassType{};
	wgpu::CommandEncoder m_wgpuCommandEncoder;
	wgpu::RenderPassEncoder m_wgpuRenderPassEncoder;
};

} // namespace sgd
