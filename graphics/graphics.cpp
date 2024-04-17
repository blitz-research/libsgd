#include "graphics.h"
#include "scenebindings.h"
#include "texture.h"

#include "shaders/uniforms.h"

#include <window/exports.h>

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
#ifndef _WIN32_WINNT_WIN10
				// Because our 1 Windows 7 user could only get Vulkan to work.
				opts.backendType = wgpu::BackendType::Vulkan; // Windows 7
#elif _MSC_VER && !_WIN64
				// Because D3D12 backend is 'freezing' in windows mode on some users' Blitz3D.
				opts.backendType = wgpu::BackendType::D3D11; // Blitz3D
#else
				opts.backendType = wgpu::BackendType::D3D12; // Windows
#endif
#elif SGD_OS_LINUX
				opts.backendType = wgpu::BackendType::Vulkan; // Linux
#elif SGD_OS_MACOS
				opts.backendType = wgpu::BackendType::Metal; // MacOS
#endif
			}
			requestWGPUDevice(opts, [&](const wgpu::Device& device) {
				m_wgpuDevice = device;
				m_wgpuSurface = createWGPUSurface(m_wgpuDevice, m_window->glfwWindow());

				auto createSwapChain = [=](CVec2u size) {
					m_canRender = size.x && size.y;
					if (!m_canRender) {
						m_wgpuSwapChain = {};
						m_colorBuffer = {};
						m_depthBuffer = {};
						return;
					}

					m_wgpuSwapChain =
						createWGPUSwapChain(m_wgpuDevice, m_wgpuSurface, size, preferredWGPUSwapChainFormat(m_wgpuDevice));

					m_colorBuffer =
						new Texture(m_window->size(), 1, sgd::TextureFormat::rgba16f, sgd::TextureFlags::renderTarget);
					m_colorBuffer->validate(this);

					m_depthBuffer =
						new Texture(m_window->size(), 1, sgd::TextureFormat::depth32f, sgd::TextureFlags::renderTarget);
					m_depthBuffer->validate(this);
				};

				createSwapChain(m_window->size());
				m_window->sizeChanged0.connect(this, [=](CVec2u size) { //
					createSwapChain(size);
				});

				ready.set(true);
			});
		},
		true);

	ready.waiteq(true);
}

void GraphicsContext::present(Texture* texture) {
	auto elapsed = sgd::micros() - m_micros;
	++m_frames;

	if (elapsed >= 1000000) {
		float seconds = (float)elapsed / 1000000.0f;
		m_fps = (int)std::round((float)m_frames / seconds);
		m_micros += 1000000;
		m_frames = 0;
	}

	if (!m_canRender) return;

	auto& wgpuTexture = texture->wgpuTexture();

	requestRender([=] {
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
	if (it != g_invalidQueue.end()) g_invalidQueue.erase(it);
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

void GraphicsResource::addDependency(CGraphicsResource* dep, CFunction<void()> func) {
	if (!dep) return;
	m_dependencies.emplace_back(dep);
	dep->invalidated.connect(this, func);
}

void GraphicsResource::removeDependency(CGraphicsResource* dep) {
	if (!dep) return;
	auto it = find(m_dependencies, dep);
	SGD_ASSERT(it != m_dependencies.end());
	m_dependencies.erase(it);
	dep->invalidated.disconnect(this);
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
