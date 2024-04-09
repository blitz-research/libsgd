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

void RenderContext::beginRenderPass(RenderPassType type, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor, float clearDepth) {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	m_renderPassType = type;

	wgpu::RenderPassDescriptor renderPassDescriptor{};

	wgpu::RenderPassColorAttachment colorAttachment{};
	if (colorBuffer) {
		colorAttachment.view = colorBuffer->wgpuTextureView();
		colorAttachment.clearValue = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};
		colorAttachment.loadOp = wgpu::LoadOp::Load;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		renderPassDescriptor.colorAttachments = &colorAttachment;
		renderPassDescriptor.colorAttachmentCount = 1;
	}

	wgpu::RenderPassDepthStencilAttachment depthAttachment{};
	if (depthBuffer) {
		depthAttachment.view = depthBuffer->wgpuTextureView();
		depthAttachment.depthClearValue = clearDepth;
		depthAttachment.depthLoadOp = wgpu::LoadOp::Load;
		depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
		renderPassDescriptor.depthStencilAttachment = &depthAttachment;
	}

	switch (m_renderPassType) {
	case RenderPassType::clear:
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
		break;
	case RenderPassType::shadow:
	case RenderPassType::opaque:
		break;
	case RenderPassType::blend:
		depthAttachment.depthLoadOp = wgpu::LoadOp::Undefined;
		depthAttachment.depthStoreOp = wgpu::StoreOp::Undefined;
		depthAttachment.depthReadOnly = true;
		break;
	}

	m_wgpuRenderPassEncoder = m_wgpuCommandEncoder.BeginRenderPass(&renderPassDescriptor);

	// Default?
//	auto size = (colorBuffer ? colorBuffer : depthBuffer)->size();
//	m_wgpuRenderPassEncoder.SetViewport(0, 0, (float)size.x, (float)size.y, 0, 1);
}

void RenderContext::render(CVector<RenderOp> ops) {
	RenderOp top;

	auto& encoder = m_wgpuRenderPassEncoder;

	encoder.SetBindGroup(1, emptyBindGroup(1)->wgpuBindGroup());

	for (auto& op : ops) {
		if (op.vertexBuffer.Get() != top.vertexBuffer.Get()) {
			if ((top.vertexBuffer = op.vertexBuffer)) encoder.SetVertexBuffer(0, top.vertexBuffer);
		}
		if (op.instanceBuffer.Get() != top.instanceBuffer.Get()) {
			if ((top.instanceBuffer = op.instanceBuffer)) encoder.SetVertexBuffer(1, top.instanceBuffer);
		}
		if (op.indexBuffer.Get() != top.indexBuffer.Get()) {
			if ((top.indexBuffer = op.indexBuffer)) encoder.SetIndexBuffer(top.indexBuffer, wgpu::IndexFormat::Uint32);
		}
		if (op.materialBindings.Get() != top.materialBindings.Get()) {
			if ((top.materialBindings = op.materialBindings)) {
				encoder.SetBindGroup(1, top.materialBindings);
			} else {
				encoder.SetBindGroup(1, emptyBindGroup(1)->wgpuBindGroup());
			}
		}
		if (op.rendererBindings.Get() != top.rendererBindings.Get()) {
			encoder.SetBindGroup(2, top.rendererBindings = op.rendererBindings);
		}
		if (op.pipeline.Get() != top.pipeline.Get()) {
			encoder.SetPipeline(top.pipeline = op.pipeline);
		}

		if (top.indexBuffer) {
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
