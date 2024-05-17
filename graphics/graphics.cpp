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
			if (opts.backendType == wgpu::BackendType::Undefined) {
#ifdef SGD_OS_WINDOWS
				OSVERSIONINFO info{sizeof(OSVERSIONINFO)};
				GetVersionEx((OSVERSIONINFO*)&info);
				if (info.dwMajorVersion > 6 ||
					(info.dwMajorVersion == 6 && info.dwMinorVersion > 1)) { // 6.2 is Windows 8, 6.1 is Windows 7.
#if _WIN64
					opts.backendType = wgpu::BackendType::D3D12; // 64 bit Windows 8+ case
#else
					// Because D3D12 backend is 'freezing' Blitz3D in windowed mode for some users.
					opts.backendType = wgpu::BackendType::D3D11; // 32 bit Windows 8+ case
#endif
				} else {
					opts.backendType = wgpu::BackendType::Vulkan; // 32/64 bit Windows 7 case
				}
#elif SGD_OS_LINUX
				opts.backendType = wgpu::BackendType::Vulkan; // Linux
#elif SGD_OS_MACOS
				opts.backendType = wgpu::BackendType::Metal; // MacOS
#endif
			}
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

					m_colorBuffer =
						new Texture(size, 1, sgd::TextureFormat::rgba16f, sgd::TextureFlags::renderTarget);
					m_depthBuffer =
						new Texture(size, 1, sgd::TextureFormat::depth32f, sgd::TextureFlags::renderTarget);

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

	++m_frames;
	auto elapsed = sgd::micros() - m_micros;
	if (elapsed >= 1000000) {
		float seconds = (float)elapsed / 1000000.0f;
		m_fps = (float)m_frames / seconds;
		m_micros += elapsed;
		m_frames = 0;
	}

	if (!m_canRender) return;

	auto& wgpuTexture = texture->wgpuTexture();

	requestRender([=] {
		struct wgpu::SurfaceTexture surfaceTexture {};
		m_wgpuSurface.GetCurrentTexture(&surfaceTexture);
		if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
			SGD_LOG << "wgpu::Surface::GetCurrentTexture() failed.";
			return;
		}
		copyTexture(m_wgpuDevice, wgpuTexture, surfaceTexture.texture);
#if !SGD_OS_EMSCRIPTEN
		m_wgpuSurface.Present();
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
