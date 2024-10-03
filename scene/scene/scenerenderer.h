#pragma once

#include "scene.h"

namespace sgd {

SGD_SHARED(SceneBindings);
SGD_SHARED(RenderContext);
SGD_SHARED(SkyboxRenderer);
SGD_SHARED(TerrainRenderer);
SGD_SHARED(ModelRenderer);
SGD_SHARED(SkinnedModelRenderer);
SGD_SHARED(SpriteRenderer);
SGD_SHARED(OverlayRenderer);
SGD_SHARED(RenderEffect);
SGD_SHARED(RenderEffectStack);

SGD_SHARED(SceneRenderer);

struct SceneRenderer : Shared {
	SGD_OBJECT_TYPE(SceneRenderer, Shared);

	SceneRenderer();

	Property<Vec4f> ambientLightColor{Vec4f(1, 1, 1, 0)};
	Property<CTexturePtr> envTexture;
	Property<Vec4f> clearColor{Vec4f(0, 0, 0, 1)};
	Property<float> clearDepth{1};

	Signal<CVec2u> renderTargetSizeChanged;

	CVec2u renderTargetSize() const {
		return m_renderTargetSize;
	}

	SceneBindings* sceneBindings() const {
		return m_sceneBindings;
	}

	SkyboxRenderer* skyboxRenderer() const {
		return m_skyboxRenderer;
	}

	TerrainRenderer* terrainRenderer() const {
		return m_terrainRenderer;
	}

	ModelRenderer* modelRenderer() const {
		return m_modelRenderer;
	}

	SkinnedModelRenderer* skinnedModelRenderer() const {
		return m_skinnedModelRenderer;
	}

	SpriteRenderer* spriteRenderer() const {
		return m_spriteRenderer;
	}

	OverlayRenderer* overlayRenderer() const {
		return m_overlayRenderer;
	}

	float RPS() const {
		return m_rps;
	}

	void add(CCamera* camera);
	void remove(CCamera* camera);

	void add(CLight* light);
	void remove(CLight* light);

	void add(RenderEffect* effect);

	void render();

	Texture* outputTexture()const;

private:
	Vec2u m_renderTargetSize;

	TexturePtr m_renderTarget;
	TexturePtr m_depthBuffer;

	Vector<CCamera*> m_cameras;

	Vector<CLight*> m_directionalLights;
	Vector<CLight*> m_pointLights;
	Vector<CLight*> m_spotLights;

	SceneBindingsPtr m_sceneBindings;
	RenderContextPtr m_renderContext;
	RenderQueuePtr m_renderQueue;
	RenderQueuePtr m_overlayQueue;

	SkyboxRendererPtr m_skyboxRenderer;
	TerrainRendererPtr m_terrainRenderer;
	ModelRendererPtr m_modelRenderer;
	SkinnedModelRendererPtr m_skinnedModelRenderer;
	SpriteRendererPtr m_spriteRenderer;
	OverlayRendererPtr m_overlayRenderer;

	RenderEffectStackPtr m_renderEffectStack;

	CCamera* m_camera{};
	Vec3r m_eye;

	wgpu::CommandEncoder m_wgpuCommandEncoder;

	static constexpr int timeStampCount = 6;

	mutable bool m_timeStampsEnabled{false};
	wgpu::QuerySet m_timeStampQueries;
	wgpu::Buffer m_timeStampBuffer;
	wgpu::Buffer m_timeStampResults;
	uint64_t m_timeStamps[timeStampCount]{};
	float m_rps = 0;

	wgpu::Future m_wgpuWorkDone{};

	void updateCameraUniforms();
	void updateLightingUniforms();

	void renderGeometry(RenderQueue* rq, RenderPassType rpassType, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
						float clearDepth, BindGroup* sceneBindings, bool enabled);

	void renderAsync();
};

} // namespace sgd
