#pragma once

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Window);
SGD_SHARED(Texture);

SGD_SHARED(GraphicsContext);
SGD_SHARED(GraphicsResource);

struct GraphicsContext : Shared {
	SGD_OBJECT_TYPE(GraphicsContext, Shared);

	explicit GraphicsContext(Window* window, wgpu::BackendType wgpuBackendType = wgpu::BackendType::Undefined);

	Window* window() const {
		return m_window;
	}

	Texture* colorBuffer() const {
		return m_colorBuffer;
	}

	Texture* depthBuffer() const {
		return m_depthBuffer;
	}

	bool canRender() const {
		return m_canRender;
	}

	void present(Texture* colorBuffer);

	float FPS() const {
		return m_fps;
	}

	// ***** Internal *****

	const wgpu::Device& wgpuDevice() const {
		return m_wgpuDevice;
	}

private:
	WindowPtr m_window;

	wgpu::Device m_wgpuDevice;
	wgpu::Surface m_wgpuSurface;
	wgpu::SwapChain m_wgpuSwapChain;

	TexturePtr m_colorBuffer;
	TexturePtr m_depthBuffer;

	bool m_canRender{};

	int64_t m_micros{};
	int m_frames{};
	float m_fps{};
};

struct GraphicsResource : Shared {
	SGD_OBJECT_TYPE(GraphicsResource, Shared);

	Signal<> invalidated;

	void validate(GraphicsContext* gc) const;

	static void validateAll(GraphicsContext* gc);

protected:
	GraphicsResource();

	~GraphicsResource() override;

	void addDependency(CGraphicsResource* dep, bool emitInvalidated = true);
	void addDependency(CGraphicsResource* dep, CFunction<void()> func);
	void removeDependency(CGraphicsResource* dep);

	void invalidate(bool emitInvalidated = false);

	virtual void onValidate(GraphicsContext* gc) const {};

private:
	inline static Vector<CGraphicsResource*> g_invalidQueue;

	Vector<CGraphicsResourcePtr> m_dependencies;

	mutable bool m_invalid{true};
	mutable bool m_emitted{false};
};

} // namespace sgd
