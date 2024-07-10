#include "rendercontext.h"

#include "../material/material.h"

namespace sgd {

wgpu::CommandEncoder RenderContext::beginRender() {
	SGD_ASSERT(isMainThread() && !m_wgpuCommandEncoder);

	return m_wgpuCommandEncoder = currentGC()->wgpuDevice().CreateCommandEncoder();
}

void RenderContext::endRender() {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	auto commandBuffer = m_wgpuCommandEncoder.Finish();
	currentGC()->wgpuDevice().GetQueue().Submit(1, &commandBuffer);

	m_wgpuCommandEncoder = {};
}

wgpu::RenderPassEncoder RenderContext::beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
									float clearDepth) {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	m_renderPassType = type;

	wgpu::RenderPassDescriptor renderPassDescriptor{};

	wgpu::RenderPassColorAttachment colorAttachment{};
	if (colorBuffer) {
		colorAttachment.view = colorBuffer->wgpuTextureView();
		colorAttachment.clearValue = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		renderPassDescriptor.colorAttachments = &colorAttachment;
		renderPassDescriptor.colorAttachmentCount = 1;
	}

	wgpu::RenderPassDepthStencilAttachment depthAttachment{};
	if (depthBuffer) {
		depthAttachment.view = depthBuffer->wgpuTextureView();
		depthAttachment.depthClearValue = clearDepth;
		depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
		depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
		renderPassDescriptor.depthStencilAttachment = &depthAttachment;
	}

	switch (m_renderPassType) {
	case RenderPassType::shadow:
	case RenderPassType::opaque:
		break;
	case RenderPassType::blend:
		colorAttachment.loadOp = wgpu::LoadOp::Load;
		depthAttachment.depthLoadOp = wgpu::LoadOp::Undefined;
		depthAttachment.depthStoreOp = wgpu::StoreOp::Undefined;
		depthAttachment.depthReadOnly = true;
		break;
	}

	m_vbuffer = nullptr;
	m_ibuffer = nullptr;
	m_material = nullptr;
	m_geometry = emptyBindGroup(BindGroupType::geometry);
	m_renderer = emptyBindGroup(BindGroupType::renderer);

	m_dirty = Dirty::material | Dirty::geometry | Dirty::renderer;

	return m_wgpuRenderPassEncoder = m_wgpuCommandEncoder.BeginRenderPass(&renderPassDescriptor);
}

void RenderContext::endRenderPass() {
	SGD_ASSERT(isMainThread() && m_wgpuRenderPassEncoder);

	m_wgpuRenderPassEncoder.End();
	m_wgpuRenderPassEncoder = {};
}

void RenderContext::validate() {
	SGD_ASSERT(isMainThread() && m_wgpuRenderPassEncoder);

	if(!bool(m_dirty)) return;

	if(bool(m_dirty & Dirty::vbuffer)) {
		if(m_vbuffer) m_wgpuRenderPassEncoder.SetVertexBuffer(0, m_vbuffer->wgpuBuffer());
	}

	if(bool(m_dirty & Dirty::ibuffer)) {
		if(m_ibuffer) m_wgpuRenderPassEncoder.SetIndexBuffer(m_ibuffer->wgpuBuffer(), wgpu::IndexFormat::Uint32);
	}

	m_dirty &= ~(Dirty::vbuffer | Dirty::ibuffer);

	if(!bool(m_dirty)) return;

	// These may require pipeline update...

	if(bool(m_dirty & Dirty::material)) {
		m_wgpuRenderPassEncoder.SetBindGroup((uint32_t)BindGroupType::material, m_material->wgpuBindGroup());
	}

	if(bool(m_dirty & Dirty::geometry)) {
		m_wgpuRenderPassEncoder.SetBindGroup((uint32_t)BindGroupType::geometry, m_geometry->wgpuBindGroup());
	}

	if(bool(m_dirty & Dirty::renderer)) {
		m_wgpuRenderPassEncoder.SetBindGroup((uint32_t)BindGroupType::renderer, m_renderer->wgpuBindGroup());
	}

	m_dirty = {};

	auto pipeline = getOrCreateRenderPipeline(m_renderPassType, m_material, m_geometry, m_renderer);

	m_wgpuRenderPassEncoder.SetPipeline(pipeline);
}

void RenderContext::render(uint32_t elementCount, uint32_t instanceCount, uint32_t firstElement) {

	validate();

	if (m_ibuffer) {
		m_wgpuRenderPassEncoder.DrawIndexed(elementCount, instanceCount, firstElement);
	} else {
		m_wgpuRenderPassEncoder.Draw(elementCount, instanceCount, firstElement);
	}
}

} // namespace sgd
