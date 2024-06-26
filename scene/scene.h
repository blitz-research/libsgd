#pragma once

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(Texture);

SGD_SHARED(Entity);
SGD_SHARED(Camera);
SGD_SHARED(Light);
SGD_SHARED(Scene);
SGD_SHARED(CollisionSpace);

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
	Property<CTexturePtr> envTexture;

	Signal<Vec2u> viewportSizeChanged;
	Signal<> beginRender;

	explicit Scene(GraphicsContext* gc);

	CVec2u viewportSize() {
		return m_viewportSize;
	}

	void clear();

	void add(Entity* entity);
	void remove(Entity* entity);

	void validate();
	void render();

	void setRenderer(RendererType type, Renderer* renderer);
	Renderer* getRenderer(RendererType type);

	SceneBindings* sceneBindings()const{
		return m_sceneBindings;
	}

	CollisionSpace* collisionSpace()const {
		return m_collisionSpace;
	}

	float RPS() const {
		return m_rps;
	}

private:
	friend class Entity;

	GraphicsContextPtr m_gc;

	Vec2u m_viewportSize;

	Vector<EntityPtr> m_entities;

	Vector<Camera*> m_cameras;
	Vector<Light*> m_directionalLights;
	Vector<Light*> m_pointLights;
	Vector<Light*> m_spotLights;

	Vector<Entity*> m_invalid;

	Array<RendererPtr, 8> m_renderers;

	CollisionSpacePtr m_collisionSpace;

	Vec3r m_eye;

	SceneBindingsPtr m_sceneBindings;

	RenderContextPtr m_renderContext;

	static constexpr int timeStampCount = 4;

	mutable bool m_timeStampsEnabled{true};
	wgpu::QuerySet m_timeStampQueries;
	wgpu::Buffer m_timeStampBuffer;
	wgpu::Buffer m_timeStampResults;
	uint64_t m_timeStamps[timeStampCount]{};
	float m_rps = 0;

	void updateCameraBindings();
	void updateLightingBindings();

	void renderASync() const;
};

} // namespace sgd
