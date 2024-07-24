#include "sprite.h"

#include "spriterenderer.h"
#include "../scene/scenerenderer.h"

namespace sgd {

Sprite::Sprite(Image* iimage):image(iimage){
}

Sprite::Sprite(const Sprite* that)
	: Entity(that),
	  image(that->image()),
	  color(that->color()),
	  frame(that->frame()) {
}

Entity* Sprite::onCopy() const {
	return new Sprite(this);
}

void Sprite::onShow() {
	scene()->sceneRenderer()->spriteRenderer()->add(this);
}

void Sprite::onHide() {
	scene()->sceneRenderer()->spriteRenderer()->remove(this);
}

} // namespace sgd
