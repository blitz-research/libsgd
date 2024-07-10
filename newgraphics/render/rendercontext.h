#pragma once

#include "../core/graphics.h"
#include "../core/bindgroup.h"

#include "renderpipeline.h"

namespace sgd {

SGD_SHARED(RenderContext);

struct RenderContext : Shared {
	SGD_OBJECT_TYPE(RenderContext, Shared);

	wgpu::CommandEncoder beginRender();

	wgpu::RenderPassEncoder beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor, float clearDepth);

	RenderPassType renderPassType() const {
		return m_renderPassType;
	}

	void setVertexBuffer(Buffer* vbuffer) {
		if (vbuffer == m_vbuffer) return;
		m_vbuffer = vbuffer;
		m_dirty |= Dirty::vbuffer;
	}

	void setIndexBuffer(Buffer* ibuffer) {
		if (ibuffer == m_ibuffer) return;
		m_ibuffer = ibuffer;
		m_dirty |= Dirty::ibuffer;
	}

	void setMaterial(CMaterial* material) {
		if (material == m_material) return;
		m_material = material;
		m_dirty |= Dirty::material;
	}

	void setGeometry(CBindGroup* geometry) {
		if (geometry == m_geometry) return;
		m_geometry = geometry;
		m_dirty |= Dirty::geometry;
	}

	void setRenderer(CBindGroup* renderer) {
		if (renderer == m_renderer) return;
		m_renderer = renderer;
		m_dirty |= Dirty::renderer;
	}

	void render(uint32_t elementCount, uint32_t instanceCount, uint32_t firstElement);

	void endRenderPass();

	void endRender();

	const wgpu::CommandEncoder& wgpuCommandEncoder() const {
		return m_wgpuCommandEncoder;
	}

	const wgpu::RenderPassEncoder& wgpuRenderPassEncoder() const {
		return m_wgpuRenderPassEncoder;
	}

private:
	enum struct Dirty { vbuffer = 1, ibuffer = 2, material = 4, geometry = 8, renderer = 16 };

	wgpu::CommandEncoder m_wgpuCommandEncoder;
	wgpu::RenderPassEncoder m_wgpuRenderPassEncoder;

	RenderPassType m_renderPassType{};

	CBuffer* m_vbuffer{};
	CBuffer* m_ibuffer{};
	CMaterial* m_material{};
	CBindGroup* m_geometry{};
	CBindGroup* m_renderer{};

	Dirty m_dirty{};

	void validate();
};

} // namespace sgd
