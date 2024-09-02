#include "sprite.h"

#include "spriterenderer.h"
#include "../scene/scenerenderer.h"

namespace sgd {

void Sprite::init() {
	color.changed.connect(nullptr,[=](CVec4f ccolor){
		auto lcolor=toLinearColor(ccolor);
		m_pmColor = {lcolor.xyz() * lcolor.w, lcolor.w};
	});
	color.changed.emit(color());
}

Sprite::Sprite(Image* iimage):image(iimage){
	init();
}

Sprite::Sprite(const Sprite* that)
	: Entity(that),
	  image(that->image()),
	  color(that->color()),
	  frame(that->frame()) {
	init();
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
