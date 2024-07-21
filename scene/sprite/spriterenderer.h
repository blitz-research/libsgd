#pragma once

#include "sprite.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(Sprite);

struct SpriteRenderer : Shared {
	SGD_OBJECT_TYPE(SpriteRenderer, Shared);

	SpriteRenderer();

	void add(Sprite* sprite);
	void remove(Sprite* sprite);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	struct Instance {
		Sprite* sprite;
		float distance{};
		Instance() = default;
		explicit Instance(Sprite* sprite) : sprite(sprite) {
		}
	};

	ImageRendererPtr m_imageRenderer;

	Vector<Instance> m_instances;
};

} // namespace sgd
