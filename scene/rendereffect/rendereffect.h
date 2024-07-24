#pragma once

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(RenderEffectStack);

SGD_SHARED(RenderEffect);

struct RenderEffect : Shared {
	SGD_OBJECT_TYPE(RenderEffect, Shared);

	RenderEffect();

	Property<bool> enabled{true};

	RenderEffectStack* stack() const {
		return m_stack;
	}

protected:
	void invalidate();

	Texture* getOrCreateRenderTarget(CVec2u size, TextureFormat format);

	virtual Texture* onValidate(Texture* sourceTexture, Texture* depthBuffer) = 0;

	virtual void onRender(RenderContext* rc, BindGroup* sceneBindings) const = 0;

private:
	friend class RenderEffectStack;

	RenderEffectStack* m_stack{};
};

struct RenderEffectStack : GraphicsResource {
	SGD_OBJECT_TYPE(RenderEffectStack, GraphicsResource);

	void setRenderTarget(Texture* renderTarget, Texture* depthBuffer);

	void add(RenderEffect* effect);

	void render(RenderContext* rc, BindGroup* sceneBindings) const;

	Texture* outputTexture() const {
		SGD_ASSERT(valid());
		return m_outputTexture;
	}

private:
	friend class RenderEffect;

	TexturePtr m_renderTarget;
	TexturePtr m_depthBuffer;

	mutable Texture* m_outputTexture{};

	Vector<RenderEffectPtr> m_effects;
	mutable Vector<RenderEffect*> m_enabledEffects;

	mutable Vector<TexturePtr> m_availRenderTargets;
	mutable Vector<Texture*> m_newRenderTargets;

	Texture* getOrCreateRenderTarget(CVec2u size, TextureFormat format);

	void onValidate() const override;
};

} // namespace sgd
