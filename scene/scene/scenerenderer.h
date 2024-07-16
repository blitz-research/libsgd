#pragma once

#include "scene.h"

namespace sgd {

SGD_SHARED(SceneBindings);
SGD_SHARED(RenderContext);
SGD_SHARED(SkyboxRenderer);
SGD_SHARED(Skybox);
SGD_SHARED(ModelRenderer);
SGD_SHARED(SkinnedModelRenderer);
SGD_SHARED(Model);
SGD_SHARED(SpriteRenderer);
SGD_SHARED(Sprite);
SGD_SHARED(OverlayRenderer);
SGD_SHARED(Overlay);

SGD_SHARED(SceneRenderer);

struct SceneRenderer : Shared {
	SGD_OBJECT_TYPE(SceneRenderer, Shared);

	SceneRenderer();

	Property<Vec4f> ambientLightColor{Vec4f(1, 1, 1, 0)};
	Property<CTexturePtr> envTexture;
	Property<Vec4f> clearColor{Vec4f(0, 0, 0, 1)};
	Property<float> clearDepth{1};

	SceneBindings* sceneBindings() const {
		return m_sceneBindings;
	}

	SkyboxRenderer* skyboxRenderer() const {
		return m_skyboxRenderer;
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

	void add(CLight* light);
	void remove(CLight* light);

	void render(CCamera* camera);

private:
	Vector<CLight*> m_directionalLights;
	Vector<CLight*> m_pointLights;
	Vector<CLight*> m_spotLights;

	SceneBindingsPtr m_sceneBindings;
	RenderContextPtr m_renderContext;
	RenderQueuePtr m_renderQueue;

	SkyboxRendererPtr m_skyboxRenderer;
	ModelRendererPtr m_modelRenderer;
	SkinnedModelRendererPtr m_skinnedModelRenderer;
	SpriteRendererPtr m_spriteRenderer;
	OverlayRendererPtr m_overlayRenderer;

	CCamera* m_camera{};
	Vec3r m_eye;

	wgpu::CommandEncoder m_wgpuCommandEncoder;

	static constexpr int timeStampCount = 4;

	mutable bool m_timeStampsEnabled{false};
	wgpu::QuerySet m_timeStampQueries;
	wgpu::Buffer m_timeStampBuffer;
	wgpu::Buffer m_timeStampResults;
	uint64_t m_timeStamps[timeStampCount]{};
	float m_rps = 0;

	wgpu::Future m_wgpuWorkDone{};

	void updateCameraUniforms();
	void updateLightingUniforms();

	void renderGeometry(RenderPassType rpassType, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
						float clearDepth, BindGroup* sceneBindings);

	void renderAsync();
};

} // namespace sgd
