#include "rendercontext.h"

#include "scenebindings.h"
#include "texture.h"

namespace sgd {

RenderContext::RenderContext(GraphicsContext* gc) : m_gc(gc) {
}

void RenderContext::beginRender() {
	SGD_ASSERT(isMainThread() && !m_wgpuCommandEncoder);

	m_wgpuCommandEncoder = m_gc->wgpuDevice().CreateCommandEncoder();
}

void RenderContext::endRender() {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	auto commandBuffer = m_wgpuCommandEncoder.Finish();
	m_gc->wgpuDevice().GetQueue().Submit(1, &commandBuffer);

	m_wgpuCommandEncoder = {};
}

void RenderContext::beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
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

	m_wgpuRenderPassEncoder = m_wgpuCommandEncoder.BeginRenderPass(&renderPassDescriptor);

	m_rop = {};
}

void RenderContext::render(CVector<RenderOp> ops) {

	auto& encoder = m_wgpuRenderPassEncoder;

	for (auto& op : ops) {
		if (op.vertexBuffer.Get() != m_rop.vertexBuffer.Get()) {
			encoder.SetVertexBuffer(0, m_rop.vertexBuffer = op.vertexBuffer);
		}
		if (op.instanceBuffer.Get() != m_rop.instanceBuffer.Get()) {
			encoder.SetVertexBuffer(1, m_rop.instanceBuffer = op.instanceBuffer);
		}
		if (op.indexBuffer.Get() != m_rop.indexBuffer.Get()) {
			if((m_rop.indexBuffer = op.indexBuffer)) encoder.SetIndexBuffer(m_rop.indexBuffer = op.indexBuffer, wgpu::IndexFormat::Uint32);
		}
		if (op.materialBindings.Get() != m_rop.materialBindings.Get()) {
			encoder.SetBindGroup(1, m_rop.materialBindings = op.materialBindings);
		}
		if (op.rendererBindings.Get() != m_rop.rendererBindings.Get()) {
			encoder.SetBindGroup(2, m_rop.rendererBindings = op.rendererBindings);
		}
		if (op.pipeline.Get() != m_rop.pipeline.Get()) {
			encoder.SetPipeline(m_rop.pipeline = op.pipeline);
		}
		if (m_rop.indexBuffer) {
			encoder.DrawIndexed(op.elementCount, op.instanceCount, op.firstElement);
		} else {
			encoder.Draw(op.elementCount, op.instanceCount, op.firstElement);
		}
	}
}

void RenderContext::endRenderPass() {
	SGD_ASSERT(isMainThread() && m_wgpuRenderPassEncoder);

	m_wgpuRenderPassEncoder.End();
	m_wgpuRenderPassEncoder = {};
}

} // namespace sgd
