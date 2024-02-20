#pragma once

#include <dawn/exports.h>

namespace sgd {

enum struct RenderPass { clear, opaque, blend };
constexpr int renderPassCount = 3;

SGD_SHARED(Window);
SGD_SHARED(Texture);
SGD_SHARED(BindGroup);

SGD_SHARED(GraphicsContext);
SGD_SHARED(GraphicsResource);

struct GraphicsContext : Shared {
	SGD_OBJECT_TYPE(GraphicsContext, Shared);

	GraphicsContext(Window* window);

	Texture* colorBuffer() const {
		return m_colorBuffer;
	}

	Texture* depthBuffer() const {
		return m_depthBuffer;
	}

	void beginRender(CVec4f clearColor, float clearDepth);

	void beginRenderPass(RenderPass renderPass);

	RenderPass renderPass() const {
		return m_renderPass;
	}

	void endRenderPass();

	void endRender();

	void present(Texture* colorBuffer);

	BindGroup* bindGroup0() const {
		return m_bindGroup0;
	}

	// ***** Internal *****

	const wgpu::Device& wgpuDevice() const {
		return m_wgpuDevice;
	}

	const wgpu::CommandEncoder& wgpuCommandEncoder() const {
		return m_wgpuCommandEncoder;
	}

	const wgpu::RenderPassEncoder& wgpuRenderPassEncoder() const {
		return m_wgpuRenderPassEncoder;
	}

private:
	WindowPtr m_window;

	TexturePtr m_colorBuffer;
	TexturePtr m_depthBuffer;

	Vec4f m_clearColor;
	float m_clearDepth;

	BindGroupPtr m_bindGroup0;

	RenderPass m_renderPass;

	wgpu::Device m_wgpuDevice;
	wgpu::Surface m_wgpuSurface;
	wgpu::SwapChain m_wgpuSwapChain;

	wgpu::CommandEncoder m_wgpuCommandEncoder;
	wgpu::RenderPassEncoder m_wgpuRenderPassEncoder;
};

struct GraphicsResource : Shared {
	SGD_OBJECT_TYPE(GraphicsResource, Shared);

	Signal<> invalidated;

	void validate(GraphicsContext* gc) const;

	static void validateAll(GraphicsContext* gc);

protected:
	GraphicsResource();

	~GraphicsResource();

	void addDependency(CGraphicsResource* dep, bool emit = true);

	void removeDependency(CGraphicsResource* dep);

	void updateDependency(CGraphicsResource* from, CGraphicsResource* to, bool emit = true);

	void invalidate(bool emit = false);

	virtual void onValidate(GraphicsContext* gc) const {};

private:
	inline static Vector<CGraphicsResource*> g_invalidQueue;

	Vector<CGraphicsResourcePtr> m_dependencies;

	mutable bool m_invalid{true};
	mutable bool m_emitted{false};
};

} // namespace sgd
