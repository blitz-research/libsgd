#include "spriterenderer.h"

namespace sgd {

void SpriteRenderer::add(Sprite* sprite) {
	m_instances.emplace_back(sprite);
}

void SpriteRenderer::remove(Sprite* sprite) {
	sgd::remove(m_instances, sprite);
}

void SpriteRenderer::update(CVec3r eye) {
}

void SpriteRenderer::render(RenderQueue* rq) const {
}

} // namespace sgd
