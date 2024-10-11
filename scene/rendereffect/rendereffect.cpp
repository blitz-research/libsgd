#include "rendereffect.h"

namespace sgd {

// ***** RenderEffect *****

RenderEffect::RenderEffect() {
	enabled.changed.connect(nullptr, [=](bool e) { invalidate(); });
}

Texture* RenderEffect::getOrCreateRenderTarget(CVec2u size, TextureFormat format) {
	return m_stack->getOrCreateRenderTarget(size, format);
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

Texture* RenderEffectStack::getOrCreateRenderTarget(CVec2u size, TextureFormat format) {
	for (Texture* target : m_availRenderTargets) {
//		if (target == m_outputTexture) continue;
//		if (size == target->size() && format == target->format()) return target;
	}
	auto target = new Texture(TextureType::e2d, size, 1, format, TextureFlags::renderTarget | TextureFlags::filter | TextureFlags::clamp);
	m_newRenderTargets.emplace_back(target);
	return target;
}

void RenderEffectStack::onValidate() const {

	// Don't free from vidmem yet
	auto oldTargets = m_availRenderTargets;

	m_availRenderTargets.clear();
	m_enabledEffects.clear();

	m_outputTexture = m_renderTarget;

	for (RenderEffect* effect : m_effects) {
		if (!effect->enabled()) continue;

		m_outputTexture = effect->onValidate(m_outputTexture, m_depthBuffer);
		m_enabledEffects.push_back(effect);

		m_availRenderTargets.insert(m_availRenderTargets.end(), m_newRenderTargets.begin(), m_newRenderTargets.end());
		m_newRenderTargets.clear();
	}
}

void RenderEffectStack::render(RenderContext* rc, BindGroup* sceneBindings) const {
	for (RenderEffect* effect : m_enabledEffects) {
		effect->onRender(rc, sceneBindings);
	}
}

} // namespace sgd
