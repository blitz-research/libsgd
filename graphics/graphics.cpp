#include "graphics.h"
#include "texture.h"

#include "shaders/uniforms.h"

#include <window/exports.h>

#include <thread>

#if SGD_OS_WINDOWS
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#endif

namespace sgd {

// ***** GraphicsContext *****

GraphicsContext::GraphicsContext(Window* window, const wgpu::BackendType wgpuBackendType) : m_window(window) {

	CondVar<bool> ready;

	runOnMainThread(
		[&] {
			wgpu::RequestAdapterOptions opts{};
			opts.backendType = wgpuBackendType;

#ifdef SGD_OS_WINDOWS
			OSVERSIONINFO info{sizeof(OSVERSIONINFO)};
			GetVersionEx((OSVERSIONINFO*)&info);
			auto windows10 = (info.dwMajorVersion == 6 && info.dwMinorVersion >= 2); // 6.2 is windows 8
			if (opts.backendType == wgpu::BackendType::Undefined) {
				if (windows10 && sizeof(void*) == 8) {
					opts.backendType = wgpu::BackendType::D3D12;
				} else if (windows10 && sizeof(void*) == 4) {
					opts.backendType = wgpu::BackendType::D3D11;
				} else {
					opts.backendType = wgpu::BackendType::Vulkan;
				}
			} else if (windows10 && sizeof(void*) == 8) {
				if (opts.backendType != wgpu::BackendType::D3D12 && //
					opts.backendType != wgpu::BackendType::D3D11 && //
					opts.backendType != wgpu::BackendType::Vulkan) {
					SGD_PANIC("Graphics backend for 64 bits Windows 10 must be D3D12 (the default), D3D11 or Vulkan");
				}
			} else if (windows10 && sizeof(void*) == 4) {
				if (opts.backendType != wgpu::BackendType::D3D11 && //
					opts.backendType != wgpu::BackendType::Vulkan) {
					SGD_PANIC("Graphics backend for 32 bits Windows 10 must be D3D11 (the default) or Vulkan");
				}
			} else {
				if (opts.backendType != wgpu::BackendType::Vulkan) {
					SGD_PANIC("Graphics backend for Windows 7 must be Vulkan");
				}
			}
#elif SGD_OS_LINUX
			if (opts.backendType == wgpu::BackendType::Undefined) {
				opts.backendType = wgpu::BackendType::Vulkan;
			} else if (opts.backendType != wgpu::BackendType::Vulkan) {
				SGD_PANIC("Graphics backend for Linux must be Vulkan");
			}
#elif SGD_OS_MACOS
			if (opts.backendType == wgpu::BackendType::Undefined) {
				opts.backendType = wgpu::BackendType::Metal;
			} else if (opts.backendType != wgpu::BackendType::Metal) {
				SGD_PANIC("Graphics backend for MacOS must be Metal");
			}
#endif

			requestWGPUDevice(opts, [&](const wgpu::Device& device) {
				m_wgpuDevice = device;
				m_wgpuSurface = createWGPUSurface(m_wgpuDevice, m_window->glfwWindow());

				auto configureSurface = [=](CVec2u size) {
					m_canRender = size.x && size.y;
					if (!m_canRender) {
						m_colorBuffer = m_depthBuffer = {};
						return;
					}
					wgpu::SurfaceConfiguration config{};
					config.device = m_wgpuDevice;
					config.width = size.x;
					config.height = size.y;
					config.format = m_wgpuSurface.GetPreferredFormat(m_wgpuDevice.GetAdapter());
					config.usage = wgpu::TextureUsage::RenderAttachment;
					config.presentMode = wgpu::PresentMode::Fifo;
					config.viewFormatCount = 0;
					config.viewFormats = nullptr;
					config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
					m_wgpuSurface.Configure(&config);

					m_colorBuffer = new Texture(size, 1, TextureFormat::rgba16f, TextureFlags::renderTarget);
					m_depthBuffer = new Texture(size, 1, TextureFormat::depth32f, TextureFlags::renderTarget);

					m_colorBuffer->validate(this);
					m_depthBuffer->validate(this);
				};

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
