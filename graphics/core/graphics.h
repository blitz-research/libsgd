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

	Signal<CVec2u> swapChainSizeChanged;

	CVec2u swapChainSize() const {
		return m_swapChainSize;
	}

	Window* window() const {
		return m_window;
	}

	float FPS() const {
		return m_fps;
	}

	void present(CTexture* texture);

	void wgpuAllocing(uint32_t size, const char* type);

	void wgpuFree(uint32_t size, const char* type);

	size_t wgpuAlloced() const {
		return m_wgpuAlloced;
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

	Vec2u m_swapChainSize;

	size_t m_wgpuAlloced{};

	int64_t m_micros{};
	int m_frames{};
	float m_fps{};

	explicit GraphicsContext(Window* window);
	~GraphicsContext() override;
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
