#include "graphics.h"

#include "texture.h"

#include <window/exports.h>

#if SGD_OS_WINDOWS
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#endif

namespace sgd {

namespace {

bool g_debugGPUAllocs = false;
auto init0 = configVarChanged("debug.gpuAllocs").connect(nullptr, [](CString value) { //
	g_debugGPUAllocs = truthiness(value);
});

bool g_debugGPUFrees = false;
auto init1 = configVarChanged("debug.gpuFrees").connect(nullptr, [](CString value) { //
	g_debugGPUFrees = truthiness(value);
});

wgpu::BackendType backendType() {

	auto cfg = getConfigVar("dawn.backendType");
#if SGD_OS_WINDOWS
	if (cfg == "D3D12")	return wgpu::BackendType::D3D12;
	if (cfg == "D3D11")	return wgpu::BackendType::D3D11;
#endif
#if SGD_OS_WINDOWS || SGD_OS_LINUX
	if (cfg == "Vulkan") return wgpu::BackendType::Vulkan;
#elif SGD_OS_MACOS
	 if (cfg == "Metal")return wgpu::BackendType::Metal;
		return wgpu::BackendType::Metal;
#endif

#if SGD_OS_WINDOWS
	OSVERSIONINFO info{sizeof(OSVERSIONINFO)};
	GetVersionEx((OSVERSIONINFO*)&info);
	if (info.dwMajorVersion > 6 || (info.dwMajorVersion == 6 && info.dwMinorVersion >= 2)) {
		// >= Windows 8
		return (sizeof(void*) == 8) ? wgpu::BackendType::D3D12 : wgpu::BackendType::D3D11; // NOLINT
	}
	// <= windows 7
	return wgpu::BackendType::Vulkan;
#elif SGD_OS_LINUX
	return wgpu::BackendType::Vulkan;
#elif SGD_OS_MACOS
	return wgpu::BackendType::Metal;
#endif
	SGD_ABORT();
}

} // namespace

// ***** GraphicsContext *****

GraphicsContext* createGC(Window* window) {
	runOnMainThread(
		[=] {
			if (GraphicsContext::g_currentGC) return;
			GraphicsContext::g_currentGC = new GraphicsContext(window);
		},
		true);
	return GraphicsContext::g_currentGC;
}

void destroyGC() {
	runOnMainThread(
		[] {
			if (!GraphicsContext::g_currentGC) return;
			GraphicsContext::g_currentGC->destroy();
			GraphicsContext::g_currentGC = nullptr;
		},
		true);
}

GraphicsContext::GraphicsContext(Window* window) : m_window(window) {

	CondVar<bool> ready;

	wgpu::RequestAdapterOptions adapterOpts{};
	adapterOpts.backendType = backendType();

	requestWGPUDevice(adapterOpts, [&](const wgpu::Device& device) {
		m_wgpuDevice = device;
		m_wgpuSurface = createWGPUSurface(m_wgpuDevice, m_window->glfwWindow());

		auto resize = [=](CVec2u size) {
			if (size == m_swapChainSize) return;

			m_swapChainSize = size;
			if (!size.x || !size.y) return;

			wgpu::SurfaceCapabilities surfCaps{};
			if (!m_wgpuSurface.GetCapabilities(m_wgpuDevice.GetAdapter(), &surfCaps)) {
				SGD_ERROR("wgpu::Surface::GetCapabilities failed");
			}
			if (!surfCaps.formatCount) {
				SGD_ERROR("wgpu::SurfaceCapabilities.formats == 0");
			}

			wgpu::SurfaceConfiguration config{};
			config.device = m_wgpuDevice;
			config.width = m_swapChainSize.x;
			config.height = m_swapChainSize.y;
			config.format = surfCaps.formats[0];
			config.usage = wgpu::TextureUsage::RenderAttachment;
			config.presentMode = wgpu::PresentMode::Fifo;
			config.viewFormatCount = 0;
			config.viewFormats = nullptr;
			config.alphaMode = wgpu::CompositeAlphaMode::Opaque;

			auto pm = getConfigVar("dawn.presentMode");
			if (pm == "Immediate") {
				config.presentMode = wgpu::PresentMode::Immediate;
			} else if (pm == "Mailbox") {
				config.presentMode = wgpu::PresentMode::Mailbox;
			} else if (pm == "FifoRelaxed") {
				config.presentMode = wgpu::PresentMode::FifoRelaxed;
			}

			m_wgpuSurface.Configure(&config);

			swapChainSizeChanged.emit(m_swapChainSize);
		};

		m_window->sizeChanged0.connect(this, [=](CVec2u size) { //
			resize(size);
		});
		resize(m_window->size());

		ready = true;
	});

	ready.waiteq(true);
}

GraphicsContext::~GraphicsContext() {
	destroy();
}

void GraphicsContext::destroy() {
	if (!m_wgpuDevice) return;
	m_wgpuDevice.Destroy();
	m_wgpuDevice = {};
}

void GraphicsContext::present(CTexture* texture) {
	if (!m_swapChainSize.x || !m_swapChainSize.y) return;

	++m_frames;
	auto elapsed = sgd::micros() - m_micros;
	if (elapsed >= 1000000) {
		float seconds = (float)elapsed / 1000000.0f;
		m_fps = (float)m_frames / seconds;
		m_micros += elapsed;
		m_frames = 0;
	}

	requestRender([=] {
		struct wgpu::SurfaceTexture surfaceTexture {};
		m_wgpuSurface.GetCurrentTexture(&surfaceTexture);

		if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
			SGD_ERROR("wgpu::Surface::GetCurrentTexture() failed.");
			return;
		}

		if (!surfaceTexture.texture) {
			SGD_ERROR("surface.texture is nullptr.");
			return;
		}

		copyTexture(m_wgpuDevice, texture->wgpuTexture(), surfaceTexture.texture);

#if !SGD_OS_EMSCRIPTEN
		m_wgpuSurface.Present();
#endif
		m_wgpuDevice.GetAdapter().GetInstance().ProcessEvents();
	});
}

void GraphicsContext::wgpuAllocing(uint32_t size, const char* type) {
	if (g_debugGPUAllocs) {
		SGD_LOG << "wgpuAllocing size:" << size << "type:" << type << " total:" << m_wgpuAlloced;
	}
	m_wgpuAlloced += size;
}

void GraphicsContext::wgpuFree(uint32_t size, const char* type) {
	if (!size) return;
	m_wgpuAlloced -= size;
	if (g_debugGPUFrees) {
		SGD_LOG << "wgpuFree size:" << size << "type:" << type << "total:" << m_wgpuAlloced;
	}
}

// ***** GraphicsResource *****

GraphicsResource::GraphicsResource() {
	g_invalidQueue.push_back(this);
}

GraphicsResource::~GraphicsResource() {
	auto it = sgd::find(g_invalidQueue, this);
	if (it != g_invalidQueue.end()) g_invalidQueue.erase(it);
}

void GraphicsResource::validate() const { // NOLINT (recursive)
	if (!m_invalid) return;
	for (CGraphicsResource* r : m_dependencies) r->validate();
	onValidate();
	m_invalid = false;
	m_emitted = false;
}

void GraphicsResource::validateAll() {
	while (!g_invalidQueue.empty()) {
		auto r = g_invalidQueue.back();
		g_invalidQueue.pop_back();
		r->validate();
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
