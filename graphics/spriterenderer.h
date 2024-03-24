#pragma once

#include "renderer.h"

namespace sgd {

SGD_SHARED(Material);
SGD_SHARED(Sprite);

struct SpriteRenderer : Renderer {
	SGD_OBJECT_TYPE(SpriteRenderer, Renderer);

	SpriteRenderer();

	void add(Sprite* sprite);

	void remove(Sprite* sprite);

private:
	struct RenderOp {
		uint32_t firstInstance;
		uint32_t instanceCount;
		wgpu::BindGroup material;
		wgpu::RenderPipeline pipeline;
	};

	BindGroupPtr m_bindGroup;

	Vector<Sprite*> m_instances;
	uint32_t m_instanceCount;
	BufferPtr m_instanceBuffer;
	uint32_t m_instanceCapacity{0};

	mutable Vector<RenderOp> m_renderOps;

	void onUpdate(CVec3f eye) override;

	void onValidate(GraphicsContext* gc) const override;

	void onRender(GraphicsContext* gc) const override;
};

}
