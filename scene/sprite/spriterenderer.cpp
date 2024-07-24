#include "spriterenderer.h"

#include "sprite.h"

#include <graphics/exports.h>

namespace sgd {

SpriteRenderer::SpriteRenderer() : m_imageRenderer(new ImageRenderer()) {
}

void SpriteRenderer::add(Sprite* sprite) {
	m_instances.emplace_back(sprite);
}

void SpriteRenderer::remove(Sprite* sprite) {
	for (auto it = m_instances.begin(); it != m_instances.end(); ++it) {
		if (it->sprite != sprite) continue;
		m_instances.erase(it);
		return;
	}
	SGD_ABORT();
}

void SpriteRenderer::update(CVec3r eye) {

	if (m_instances.empty()) return;

	for (auto& inst : m_instances) {
		inst.distance = (float)length(inst.sprite->worldPosition() - eye);
	}

	auto cmp = [=](const Instance& lhs, const Instance& rhs) { return lhs.distance > rhs.distance; };

	std::sort(m_instances.begin(), m_instances.end(), cmp);

	auto instp = m_imageRenderer->lockInstances(m_instances.size());

	for (auto& inst : m_instances) {
		instp->worldMatrix = inst.sprite->worldMatrix();
		instp->worldMatrix.t.xyz() -= eye;
		instp->color = inst.sprite->color();
		instp->frame = inst.sprite->frame();
		++instp;
	}

	m_imageRenderer->unlockInstances();
}

void SpriteRenderer::render(RenderQueue* rq) const {

	if (m_instances.empty()) return;

	CImage* image = m_instances.front().sprite->image();
	uint32_t first = 0;
	uint32_t count = 0;

	for (auto& inst : m_instances) {
		if (inst.sprite->image() != image) {
			m_imageRenderer->render(rq, image, first, count);
			first += count;
			image = inst.sprite->image();
			count = 1;
		} else {
			++count;
		}
	}

	if (count) m_imageRenderer->render(rq, image, first, count);
}

} // namespace sgd
