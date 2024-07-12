#pragma once

#include "sprite.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(Sprite);

struct SpriteRenderer : Shared {
	SGD_OBJECT_TYPE(SpriteRenderer, Shared);

	void add(Sprite* sprite);
	void remove(Sprite* sprite);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	Vector<Sprite*> m_instances;
};

}
