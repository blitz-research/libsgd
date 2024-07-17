#include "rendereffect.h"

namespace sgd {

// ***** RenderEffect *****

Texture* RenderEffect::getOrCreateRenderTarget(CVec2u size, TextureFormat format) {
	return m_stack->getOrCreateRenderTarget(size, format, this);
}

void RenderEffect::create(RenderEffectStack* stack, RenderEffect* source) {
	m_stack = stack;
	m_source = source;
}

// ***** RenderEffectStack *****

void RenderEffectStack::setRenderTarget(Texture* renderTarget, Texture* depthBuffer) {
	m_renderTarget = renderTarget;
	m_depthBuffer = depthBuffer;
	m_dirty = true;
}

void RenderEffectStack::add(RenderEffect* effect) {
	effect->create(this, !m_effects.empty() ? m_effects.back() : nullptr);
	m_effects.emplace_back(effect);
	m_dirty = true;
}

void RenderEffectStack::validate() {
	if (!m_dirty) return;
	m_dirty = false;
	m_renderTargets.clear();
	m_renderTargets.push_back(m_renderTarget);
	for (RenderEffect* effect : m_effects) {
		effect->onValidate();
	}
}

void RenderEffectStack::render(RenderContext* rc, BindGroup* sceneBindings) {
	SGD_ASSERT(!m_dirty);
	for (RenderEffect* effect : m_effects) {
		effect->onRender(rc, sceneBindings);
	}
}

Texture* RenderEffectStack::getOrCreateRenderTarget(CVec2u size, TextureFormat format, RenderEffect* effect) {
	for (Texture* target : m_renderTargets) {
		if (target == effect->sourceTexture()) continue;
		if (size == target->size() || format == target->format()) return target;
	}
	auto target = new Texture(size, 1, format, TextureFlags::renderTarget);
	m_renderTargets.emplace_back(target);
	return target;
}

} // namespace sgd
