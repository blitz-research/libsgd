#pragma once

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Window);
SGD_SHARED(Texture);

SGD_SHARED(GraphicsContext);
SGD_SHARED(GraphicsResource);

struct GraphicsContext : Shared {
	SGD_OBJECT_TYPE(GraphicsContext, Shared);

	friend GraphicsContext* createGC(Window* window);
	friend void destroyGC();

	inline friend GraphicsContext* currentGC() {
		return g_currentGC;
	}

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

	const wgpu::Device& wgpuDevice() const {
		SGD_ASSERT(isMainThread());
		return m_wgpuDevice;
	}

private:
	inline static GraphicsContextPtr g_currentGC;

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

	explicit GraphicsContext(Window* window);
	~GraphicsContext();
};

GraphicsContext* createGC(Window* window);
void destroyGC();
GraphicsContext* currentGC();

struct GraphicsResource : Shared {
	SGD_OBJECT_TYPE(GraphicsResource, Shared);

	Signal<> invalidated;

	void validate() const;

	static void validateAll();

protected:
	GraphicsResource();

	~GraphicsResource() override;

	void addDependency(CGraphicsResource* dep, bool emitInvalidated = true);
	void addDependency(CGraphicsResource* dep, CFunction<void()> func);
	void removeDependency(CGraphicsResource* dep);

	void invalidate(bool emitInvalidated = false);

	virtual void onValidate() const {};

	bool valid() const {
		return !m_invalid;
	}

private:
	inline static Vector<CGraphicsResource*> g_invalidQueue;

	Vector<CGraphicsResourcePtr> m_dependencies;

	mutable bool m_invalid{true};
	mutable bool m_emitted{false};
};

} // namespace sgd
