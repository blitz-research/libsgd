#pragma once

#include "material.h"

namespace sgd {

enum struct SpriteViewMode {
	fixed = 1,	 // locked to camera, 2d style
	free = 2,	 // free of camera, billboard style
	upright = 3, // upright, tree style
};

SGD_SHARED(Image);

struct Image : Shared {
	SGD_OBJECT_TYPE(Image, Shared);

	Image();

	Property<Texture*> frames;

	Property<BlendMode> blendMode;

	Property<Rectf> spriteRect{Rectf(-.5f, .5f)};

	Property<SpriteViewMode> spriteViewMode{SpriteViewMode::fixed};

	Property<Vec2f> drawHandle{};

	Vec2u frameSize() const {
		return frames()->size();
	}

	Rectf drawRect() const {
		return m_drawRect;
	}

	Material* material() const {
		return m_material;
	}

private:
	MaterialPtr m_material;
	Rectf m_drawRect;
};

} // namespace sgd
