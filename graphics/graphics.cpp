#include "graphics.h"
#include "renderpipeline.h"
#include "scenebindings.h"
#include "texture.h"

#include "shaders/uniforms.h"

#include <window/exports.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace sgd {

// ***** GraphicsContext *****

GraphicsContext::GraphicsContext(Window* window, const wgpu::BackendType wgpuBackendType) : m_window(window) {

	CondVar<bool> ready;

	runOnMainThread(
		[&] {
			wgpu::RequestAdapterOptions opts{};
			opts.backendType = wgpuBackendType;
			if (opts.backendType == wgpu::BackendType::Undefined) {
#ifdef SGD_OS_WINDOWS
				opts.backendType = wgpu::BackendType::D3D12;
#elif SGD_OS_LINUX
				opts.backendType = wgpu::BackendType::Vulkan;
#elif SGD_OS_MACOS
				opts.backendType = wgpu::BackendType::Metal;
#endif
			}
			requestWGPUDevice(opts, [&](const wgpu::Device& device) {
				m_wgpuDevice = device;
				m_wgpuSurface = createWGPUSurface(m_wgpuDevice, m_window->glfwWindow());
				m_wgpuSwapChain = createWGPUSwapChain(m_wgpuDevice, m_wgpuSurface, m_window->size(),
													  preferredWGPUSwapChainFormat(m_wgpuDevice));

				m_colorBuffer = new Texture(m_window->size(), 1, sgd::TextureFormat::rgba16f, sgd::TextureFlags::renderTarget);
				m_depthBuffer = new Texture(m_window->size(), 1, sgd::TextureFormat::depth32f, sgd::TextureFlags::renderTarget);

				m_sceneBindings = new SceneBindings();

				m_window->sizeChanged.connect(this, [=](CVec2u size) {
					m_wgpuSwapChain =
						createWGPUSwapChain(m_wgpuDevice, m_wgpuSurface, size, preferredWGPUSwapChainFormat(m_wgpuDevice));

					m_colorBuffer->resize(size);
					m_depthBuffer->resize(size);
				});

				ready.set(true);
			});
		},
		true);

	ready.waiteq(true);
}

void GraphicsContext::beginRender(CVec4f clearColor, float clearDepth) {
	SGD_ASSERT(isMainThread() && !m_wgpuCommandEncoder);

	m_clearColor = clearColor;
	m_clearDepth = clearDepth;

	m_wgpuCommandEncoder = m_wgpuDevice.CreateCommandEncoder();
}

void GraphicsContext::beginRenderPass(RenderPass rpass) {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	m_renderPass = rpass;

	wgpu::RenderPassColorAttachment colorAttachment;
	colorAttachment.view = m_colorBuffer->wgpuTextureView();
	colorAttachment.clearValue = {m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w};
	colorAttachment.loadOp = wgpu::LoadOp::Load;
	colorAttachment.storeOp = wgpu::StoreOp::Store;

	wgpu::RenderPassDepthStencilAttachment depthAttachment;
	depthAttachment.view = m_depthBuffer->wgpuTextureView();
	depthAttachment.depthClearValue = m_clearDepth;
	depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
	depthAttachment.depthStoreOp = wgpu::StoreOp::Store;

	wgpu::RenderPassDescriptor renderPassDescriptor;
	renderPassDescriptor.colorAttachments = &colorAttachment;
	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.depthStencilAttachment = &depthAttachment;

	switch (m_renderPass) {
	case RenderPass::clear:
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
		break;
	case RenderPass::opaque:
		break;
	case RenderPass::blend:
		depthAttachment.depthLoadOp = wgpu::LoadOp::Undefined;
		depthAttachment.depthStoreOp = wgpu::StoreOp::Undefined;
		depthAttachment.depthReadOnly = true;
		break;
	}
	m_wgpuRenderPassEncoder = m_wgpuCommandEncoder.BeginRenderPass(&renderPassDescriptor);

	// Default?
	m_wgpuRenderPassEncoder.SetViewport(0, 0, (float)m_colorBuffer->size().x, (float)m_colorBuffer->size().y, 0, 1);

	m_wgpuRenderPassEncoder.SetBindGroup(0, m_sceneBindings->bindGroup()->wgpuBindGroup());
}

void GraphicsContext::endRenderPass() {
	SGD_ASSERT(isMainThread() && m_wgpuRenderPassEncoder);

	m_wgpuRenderPassEncoder.End();
	m_wgpuRenderPassEncoder = {};
}

void GraphicsContext::endRender() {
	SGD_ASSERT(isMainThread() && m_wgpuCommandEncoder && !m_wgpuRenderPassEncoder);

	auto commandBuffer = m_wgpuCommandEncoder.Finish();
	m_wgpuDevice.GetQueue().Submit(1, &commandBuffer);

	m_wgpuCommandEncoder = {};
}

void GraphicsContext::present(Texture* texture) {

	auto wgpuTexture = texture->wgpuTexture();

	requestRender([=] {
		SGD_ASSERT(!m_wgpuCommandEncoder);
		copyTexture(m_wgpuDevice, wgpuTexture, m_wgpuSwapChain.GetCurrentTexture());
#if !SGD_OS_EMSCRIPTEN
		m_wgpuSwapChain.Present();
#endif
	});
}

// ***** GraphicsResource *****

GraphicsResource::GraphicsResource() {
	g_invalidQueue.push_back(this);
}

GraphicsResource::~GraphicsResource() {
	auto it = sgd::find(g_invalidQueue, this);
	if(m_invalid) {
		SGD_ASSERT(it!=g_invalidQueue.end());
		g_invalidQueue.erase(it);
	}else{
		SGD_ASSERT(it==g_invalidQueue.end());
	}
}

void GraphicsResource::validate(GraphicsContext* gc) const { // NOLINT (recursive)
	if (!m_invalid) return;
	for (CGraphicsResource* r : m_dependencies) r->validate(gc);
	onValidate(gc);
	m_invalid = false;
	m_emitted = false;
}

void GraphicsResource::validateAll(GraphicsContext* gc) {
	while (!g_invalidQueue.empty()) {
		auto r = g_invalidQueue.back();
		g_invalidQueue.pop_back();
		r->validate(gc);
	}
}

void GraphicsResource::addDependency(CGraphicsResource* dep, bool emit) {
	if (!dep) return;
	m_dependencies.emplace_back(dep);
	dep->invalidated.connect(this, [=] { //
		invalidate(emit);
	});
}

void GraphicsResource::removeDependency(CGraphicsResource* dep) {
	if (!dep) return;
	auto it = find(m_dependencies, dep);
	SGD_ASSERT(it != m_dependencies.end());
	m_dependencies.erase(it);
	dep->invalidated.disconnect(this);
}

void GraphicsResource::updateDependency(CGraphicsResource* from, CGraphicsResource* to, bool emit) {
	removeDependency(from);
	addDependency(to, emit);
}

void GraphicsResource::invalidate(bool emit) {
	if (!m_invalid) {
		m_invalid = true;
		g_invalidQueue.push_back(this);
	}
	if (!emit || m_emitted) return;
	m_emitted = true;
	invalidated.emit();
}

} // namespace sgd
