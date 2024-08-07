#include "rendercontext.h"

#include "../material/material.h"

namespace sgd {

namespace {

bool g_debugOps;

}

wgpu::CommandEncoder RenderContext::beginRender() {
	SGD_ASSERT(isMainThread() && !m_wgpuCommandEncoder);

	if(g_debugOps) SGD_LOG << "Begin render";
	return m_wgpuCommandEncoder = currentGC()->wgpuDevice().CreateCommandEncoder();
}

void RenderContext::endRender() {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	if(g_debugOps) SGD_LOG << "End render";
	auto commandBuffer = m_wgpuCommandEncoder.Finish();
	currentGC()->wgpuDevice().GetQueue().Submit(1, &commandBuffer);

	m_wgpuCommandEncoder = {};
}

wgpu::RenderPassEncoder RenderContext::beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
									float clearDepth, BindGroup* sceneBindings) {

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

	if(g_debugOps) SGD_LOG<< "Begin render pass";
	m_wgpuRenderPassEncoder = m_wgpuCommandEncoder.BeginRenderPass(&renderPassDescriptor);

	m_wgpuRenderPassEncoder.SetBindGroup(0, sceneBindings->wgpuBindGroup());
	m_wgpuRenderPassEncoder.SetBindGroup(1, emptyBindGroup(BindGroupType::material)->wgpuBindGroup());
	m_wgpuRenderPassEncoder.SetBindGroup(2, emptyBindGroup(BindGroupType::geometry)->wgpuBindGroup());
	m_wgpuRenderPassEncoder.SetBindGroup(3, emptyBindGroup(BindGroupType::renderer)->wgpuBindGroup());

	m_rop = {};

	return m_wgpuRenderPassEncoder;
}

void RenderContext::endRenderPass() {
	SGD_ASSERT(isMainThread() && m_wgpuRenderPassEncoder);

	if(g_debugOps) SGD_LOG<< "End render pass";
	m_wgpuRenderPassEncoder.End();

	m_wgpuRenderPassEncoder = {};
}

void RenderContext::render(CVector<RenderOp>& ops) {

	auto& encoder = m_wgpuRenderPassEncoder;

	for (auto& op : ops) {
		if (op.vbuffer.Get() != m_rop.vbuffer.Get()) {
			if(g_debugOps) SGD_LOG << "Set vertex buffer";
			if((m_rop.vbuffer = op.vbuffer)) encoder.SetVertexBuffer(0, m_rop.vbuffer);
		}
		if (op.ibuffer.Get() != m_rop.ibuffer.Get()) {
			if(g_debugOps) SGD_LOG << "Set index buffer";
			if((m_rop.ibuffer = op.ibuffer)) encoder.SetIndexBuffer(m_rop.ibuffer, wgpu::IndexFormat::Uint32);
		}
		if (op.material.Get() != m_rop.material.Get()) {
			if(g_debugOps) SGD_LOG << "Set material bind group";
			encoder.SetBindGroup(1, m_rop.material = op.material);
		}
		if (op.geometry.Get() != m_rop.geometry.Get()) {
			if(g_debugOps) SGD_LOG << "Set geometry bind group";
			encoder.SetBindGroup(2, m_rop.geometry = op.geometry);
		}
		if (op.renderer.Get() != m_rop.renderer.Get()) {
			if(g_debugOps) SGD_LOG << "Set renderer bind group";
			encoder.SetBindGroup(3, m_rop.renderer = op.renderer);
		}
		if (op.pipeline.Get() != m_rop.pipeline.Get()) {
			if(g_debugOps) SGD_LOG << "Set render pipeline";
			encoder.SetPipeline(m_rop.pipeline = op.pipeline);
		}
		if (m_rop.ibuffer) {
			if(g_debugOps) SGD_LOG << "Draw indexed";
			encoder.DrawIndexed(op.elementCount, op.instanceCount, op.firstElement, 0, op.firstInstance);
		} else {
			if(g_debugOps) SGD_LOG << "Draw";
			encoder.Draw(op.elementCount, op.instanceCount, op.firstElement, op.firstInstance);
		}
	}
}

} // namespace sgd
