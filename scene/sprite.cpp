#include "sprite.h"

#include "spriterenderer.h"
#include "scene.h"

namespace sgd {

Sprite::Sprite(const Sprite* that)
	: Entity(that),
	  image(that->image()),
	  color(that->color()),
	  frame(that->frame()) {
}

Entity* Sprite::onCopy() const {
	return new Sprite(this);
}

void Sprite::onCreate() {
	if (scene()->getRenderer(RendererType::sprite)) return;
	scene()->setRenderer(RendererType::sprite, new SpriteRenderer());
}

void Sprite::onShow() {
	scene()->getRenderer(RendererType::sprite)->as<SpriteRenderer>()->add(this);
}

void Sprite::onHide() {
	scene()->getRenderer(RendererType::sprite)->as<SpriteRenderer>()->remove(this);
}

} // namespace sgd
