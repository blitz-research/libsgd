#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Texture);

SGD_SHARED(Entity);
SGD_SHARED(Camera);
SGD_SHARED(Light);
SGD_SHARED(Renderer)
SGD_SHARED(GraphicsContext);

SGD_SHARED(Scene);

enum struct RendererType {
	skybox,
	model,
	skinnedModel,
	sprite,
	//8 MAX!
};

struct CameraUniforms;
struct LightingUniforms;

struct Scene : Shared {
	SGD_OBJECT_TYPE(Scene, Shared);

	Property<Vec4f> clearColor{Vec4f(0, 0, 0, 1)};
	Property<float> clearDepth{1};

	Property<Vec4f> ambientLightColor{Vec4f(1, 1, 1, 0)};
	Property<TexturePtr> envTexture;

	Scene(GraphicsContext* gc);

	void clear();

	void add(Entity* entity);
	void remove(Entity* entity);

	void setRenderer(RendererType type, Renderer* renderer);
	Renderer* getRenderer(RendererType type);

	void render();

private:
	friend class Entity;

	GraphicsContextPtr m_gc;

	Vector<EntityPtr> m_entities;
	Vector<CameraPtr> m_cameras;
	Vector<LightPtr> m_lights;

	Array<RendererPtr,8> m_renderers;

	void updateCameraUniforms(CameraUniforms& uniforms) const;
	void updateLightingUniforms(LightingUniforms& uniforms) const;

	void renderASync();
};

} // namespace sgd
