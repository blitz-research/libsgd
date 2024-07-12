#include "graphics.h"

#include "texture.h"

#include <window/exports.h>

#if SGD_OS_WINDOWS
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#endif

namespace sgd {

// ***** GraphicsContext *****

GraphicsContext::GraphicsContext(Window* window) : m_window(window) {
	if (g_currentGC) SGD_ABORT();

	CondVar<bool> ready;

	runOnMainThread(
		[&] {
			auto backendType = wgpu::BackendType::Undefined;
			auto bt = getConfigVar("dawn.backendType");
			if (bt == "D3D12") {
#if SGD_OS_WINDOWS
				backendType = wgpu::BackendType::D3D12;
#endif
			} else if (bt == "D3D11") {
#if SGD_OS_WINDOWS
				backendType = wgpu::BackendType::D3D11;
#endif
			} else if (bt == "Vulkan") {
#if SGD_OS_WINDOWS || SGD_OS_LINUX
				backendType = wgpu::BackendType::Vulkan;
#endif
			} else if (bt == "Metal") {
#if SGD_OS_MACOS
				backendType = wgpu::BackendType::Metal;
#endif
			}

			if (backendType == wgpu::BackendType::Undefined) {
#if SGD_OS_WINDOWS
				OSVERSIONINFO info{sizeof(OSVERSIONINFO)};
				GetVersionEx((OSVERSIONINFO*)&info);
				if (info.dwMajorVersion == 6 && info.dwMinorVersion >= 2) { // 6.2 is windows 8
					backendType = (sizeof(void*) == 8) ? wgpu::BackendType::D3D12 : wgpu::BackendType::D3D11; // NOLINT
				} else {
					backendType = wgpu::BackendType::Vulkan;
				}
#elif SGD_OS_LINUX
				backendType = wgpu::BackendType::Vulkan;
#elif SGD_OS_MACOS
				backendType = wgpu::BackendType::Metal;
#endif
			}

			wgpu::RequestAdapterOptions opts{};
			opts.backendType = backendType;

			requestWGPUDevice(opts, [&](const wgpu::Device& device) {
				m_wgpuDevice = device;
				m_wgpuSurface = createWGPUSurface(m_wgpuDevice, m_window->glfwWindow());

				auto configureSurface = [=](CVec2u size) {
					m_canRender = size.x && size.y;
					if (!m_canRender) {
						m_colorBuffer = m_depthBuffer = {};
						return;
					}

					wgpu::SurfaceCapabilities surfCaps{};
					if (!m_wgpuSurface.GetCapabilities(m_wgpuDevice.GetAdapter(), &surfCaps)) {
						SGD_PANIC("wgpu::Surface::GetCapabilities failed");
					}
					if (!surfCaps.formatCount) {
						SGD_PANIC("wgpu::SurfaceCapabilities.formats == 0");
					}

					wgpu::SurfaceConfiguration config{};
					config.device = m_wgpuDevice;
					config.width = size.x;
					config.height = size.y;
					config.format = surfCaps.formats[0];
					config.usage = wgpu::TextureUsage::RenderAttachment;
					config.presentMode = wgpu::PresentMode::Fifo;
					config.viewFormatCount = 0;
					config.viewFormats = nullptr;
					config.alphaMode = wgpu::CompositeAlphaMode::Opaque;

					auto pm = getConfigVar("dawn.presentMode");

					if(pm=="Immediate") {
						config.presentMode = wgpu::PresentMode::Immediate;
					}else if(pm=="Mailbox") {
						config.presentMode = wgpu::PresentMode::Mailbox;
					}else if(pm=="FifoRelaxed") {
						config.presentMode = wgpu::PresentMode::FifoRelaxed;
					}

					m_wgpuSurface.Configure(&config);

					m_colorBuffer = new Texture(size, 1, TextureFormat::rgba16f, TextureFlags::renderTarget);
					m_depthBuffer = new Texture(size, 1, TextureFormat::depth32f, TextureFlags::renderTarget);

					m_colorBuffer->validate();
					m_depthBuffer->validate();
				};

				if (!g_currentGC) g_currentGC = this;

				m_window->sizeChanged0.connect(this, [=](CVec2u size) { //
					configureSurface(size);
				});
				configureSurface(m_window->size());

				ready = true;
			});
		},
		true);

	ready.waiteq(true);
}

void GraphicsContext::present(Texture* texture) {

	if (!m_canRender) return;

	++m_frames;
	auto elapsed = sgd::micros() - m_micros;
	if (elapsed >= 1000000) {
		float seconds = (float)elapsed / 1000000.0f;
		m_fps = (float)m_frames / seconds;
		m_micros += elapsed;
		m_frames = 0;
	}

	auto& wgpuTexture = texture->wgpuTexture();

	requestRender([=] {
#if !SGD_OS_EMSCRIPTEN
		m_wgpuDevice.GetAdapter().GetInstance().ProcessEvents();
#endif
		struct wgpu::SurfaceTexture surfaceTexture {};
		m_wgpuSurface.GetCurrentTexture(&surfaceTexture);
		if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
			SGD_PANIC("wgpu::Surface::GetCurrentTexture() failed.");
			return;
		}
		if (!surfaceTexture.texture) {
			SGD_PANIC("surface.texture is nullptr.");
			return;
		}
		copyTexture(m_wgpuDevice, wgpuTexture, surfaceTexture.texture);
#if !SGD_OS_EMSCRIPTEN
		m_wgpuSurface.Present();
		m_wgpuDevice.GetAdapter().GetInstance().ProcessEvents();
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
