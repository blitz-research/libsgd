#pragma once

#include "rendercontext.h"
#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Texture);

SGD_SHARED(Entity);
SGD_SHARED(Camera);
SGD_SHARED(Light);
SGD_SHARED(Renderer)
SGD_SHARED(GraphicsContext);
SGD_SHARED(SceneBindings);
SGD_SHARED(RenderContext);
SGD_SHARED(Scene);

enum struct RendererType {
	skybox,
	model,
	skinnedModel,
	sprite,
	overlay,
	// 8 MAX!
};

struct CameraUniforms;
struct LightingUniforms;

struct Scene : Shared {
	SGD_OBJECT_TYPE(Scene, Shared);

	Property<Vec4f> clearColor{Vec4f(0, 0, 0, 1)};
	Property<float> clearDepth{1};

	Property<Vec4f> ambientLightColor{Vec4f(1, 1, 1, 0)};
	Property<TexturePtr> envTexture;

	Signal<Vec2u> viewportSizeChanged;
	Signal<> beginRender;

	explicit Scene(GraphicsContext* gc);

	CVec2u viewportSize() {
		return m_viewportSize;
	}

	void clear();

	void add(Entity* entity);
	void remove(Entity* entity);

	void setRenderer(RendererType type, Renderer* renderer);
	Renderer* getRenderer(RendererType type);

	void render();

private:
	friend class Entity;

	GraphicsContextPtr m_gc;

	Vec2u m_viewportSize;

	SceneBindingsPtr m_sceneBindings;
	Array<SceneBindingsPtr, 6> m_shadowBindings;

	Vector<EntityPtr> m_entities;
	Vector<CameraPtr> m_cameras;
	Vector<LightPtr> m_lights;
	Array<RendererPtr, 8> m_renderers;

	Vec3r m_eye;

	Vector<Light*> m_pointShadowLights;
	TexturePtr m_pointShadowTexture;

	Vector<TexturePtr> m_pointShadowTextureFaces;

	RenderContextPtr m_renderContext;

	void updateCameraBindings();
	void updateLightingBindings();

	void renderPointLightShadowMaps() const;
	void renderGeometry(RenderPassType renderPassType) const;

	void renderASync() const;
};

} // namespace sgd
