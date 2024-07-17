#include "rendereffect.h"

namespace sgd {

// ***** RenderEffect *****

Texture* RenderEffect::getOrCreateRenderTarget(CVec2u size, TextureFormat format) {
	return m_stack->getOrCreateRenderTarget(size, format, this);
}

void RenderEffect::invalidate() {
	if(m_stack) m_stack->invalidate();
}

void RenderEffect::create(RenderEffectStack* stack, RenderEffect* source) {
	m_stack = stack;
	m_source = source;
}

// ***** RenderEffectStack *****

void RenderEffectStack::setRenderTarget(Texture* renderTarget, Texture* depthBuffer) {
	m_renderTarget = renderTarget;
	m_depthBuffer = depthBuffer;
	invalidate();
}

void RenderEffectStack::add(RenderEffect* effect) {
	effect->create(this, !m_effects.empty() ? m_effects.back() : nullptr);
	m_effects.emplace_back(effect);
	invalidate();
}

Texture* RenderEffectStack::getOrCreateRenderTarget(CVec2u size, TextureFormat format, RenderEffect* effect) {
	for (Texture* target : m_renderTargets) {
		if (target == effect->sourceTexture()) continue;
		if (size == target->size() && format == target->format()) return target;
	}
	auto target = new Texture(size, 1, format, TextureFlags::renderTarget|TextureFlags::filter|TextureFlags::clamp);
	m_renderTargets.emplace_back(target);
	return target;
}

void RenderEffectStack::render(RenderContext* rc, BindGroup* sceneBindings) const {
	for (RenderEffect* effect : m_effects) {
		effect->onRender(rc, sceneBindings);
	}
}

void RenderEffectStack::onValidate() const {
	m_renderTargets.clear();
	m_renderTargets.push_back(m_renderTarget);
	for (RenderEffect* effect : m_effects) {
		effect->onValidate();
	}
}

} // namespace sgd
