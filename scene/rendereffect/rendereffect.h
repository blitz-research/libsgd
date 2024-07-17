#pragma once

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(RenderEffectStack);

SGD_SHARED(RenderEffect);

struct RenderEffect : Shared {
	SGD_OBJECT_TYPE(RenderEffect, Shared);

	RenderEffect() = default;

	RenderEffectStack* stack() const {
		return m_stack;
	}

	RenderEffect* source() const {
		return m_source;
	}

	Texture* renderTarget() const {
		return m_renderTarget;
	}

	Texture* sourceTexture() const;

protected:
	mutable TexturePtr m_renderTarget;

	Texture* getOrCreateRenderTarget(CVec2u size, TextureFormat format);

	virtual void onValidate() = 0;

	virtual void onRender(RenderContext* rc, BindGroup* sceneBindings) const = 0;

private:
	friend class RenderEffectStack;

	RenderEffectStack* m_stack{};

	RenderEffect* m_source{};

	void create(RenderEffectStack* stack, RenderEffect* source);
};

struct RenderEffectStack : GraphicsResource {
	SGD_OBJECT_TYPE(RenderEffectStack, GraphicsResource);

	void setRenderTarget(Texture* renderTarget, Texture* depthBuffer);

	Texture* renderTarget() const {
		return m_renderTarget;
	}

	Texture* depthBuffer() const {
		return m_depthBuffer;
	}

	Texture* outputTexture() const {
		return !m_effects.empty() ? m_effects.back()->renderTarget() : m_renderTarget.get();
	}

	void add(RenderEffect* effect);

	void validate();

	void render(RenderContext* rc, BindGroup* sceneBindings);

private:
	friend class RenderEffect;

	TexturePtr m_renderTarget;
	TexturePtr m_depthBuffer;

	Vector<TexturePtr> m_renderTargets;
	Vector<RenderEffectPtr> m_effects;

	bool m_dirty{};

	Texture* getOrCreateRenderTarget(CVec2u size, TextureFormat format, RenderEffect* effect);
};

inline Texture* RenderEffect::sourceTexture() const {
	return m_source ? m_source->m_renderTarget : m_stack->m_renderTarget;
}

} // namespace sgd
