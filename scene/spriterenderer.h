#pragma once

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(Sprite);

// TODO: Move this to graphics module
struct SpriteRenderer : Renderer {
	SGD_OBJECT_TYPE(SpriteRenderer, Renderer);

	SpriteRenderer();

	void add(Sprite* sprite);

	void remove(Sprite* sprite);

private:
	Vector<Sprite*> m_instances;
	uint32_t m_instanceCount{};

	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCapacity;
	BufferPtr m_instanceBuffer;

	void onUpdate(CVec3r eye) override;

	void onValidate(GraphicsContext* gc) const override;
};

}
