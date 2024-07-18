#include "rendereffect.h"

namespace sgd {

// ***** RenderEffect *****

RenderEffect::RenderEffect() {
	enabled.changed.connect(nullptr, [=](bool e) { invalidate(); });
}

Texture* RenderEffect::getOrCreateRenderTarget(CVec2u size, TextureFormat format) {
	return m_stack->getOrCreateRenderTarget(size, format, this);
}

void RenderEffect::invalidate() {
	if (m_stack) m_stack->invalidate();
}

// ***** RenderEffectStack *****

void RenderEffectStack::setRenderTarget(Texture* renderTarget, Texture* depthBuffer) {
	m_renderTarget = renderTarget;
	m_depthBuffer = depthBuffer;
	invalidate();
}

void RenderEffectStack::add(RenderEffect* effect) {
	SGD_ASSERT(!effect->m_stack);
	effect->m_stack = this;
	m_effects.emplace_back(effect);
	invalidate();
}

Texture* RenderEffectStack::getOrCreateRenderTarget(CVec2u size, TextureFormat format, RenderEffect* effect) {
	for (Texture* target : m_renderTargets) {
		if (target == m_outputTexture) continue;
		if (size == target->size() && format == target->format()) return target;
	}
	auto target = new Texture(size, 1, format, TextureFlags::renderTarget | TextureFlags::filter | TextureFlags::clamp);
	m_renderTargets.emplace_back(target);
	return target;
}

void RenderEffectStack::onValidate() const {

	auto prevTargets = m_renderTargets;

	m_renderTargets.clear();
	m_renderTargets.push_back(m_renderTarget);

	m_enabledEffects.clear();
	m_outputTexture = m_renderTarget;
	for (RenderEffect* effect : m_effects) {
		if (!effect->enabled()) continue;
		m_outputTexture = effect->onValidate(m_outputTexture);
		m_enabledEffects.push_back(effect);
	}
}

void RenderEffectStack::render(RenderContext* rc, BindGroup* sceneBindings) const {
	for (RenderEffect* effect : m_enabledEffects) {
		effect->onRender(rc, sceneBindings);
	}
}

} // namespace sgd
