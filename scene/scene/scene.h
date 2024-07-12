#pragma once

#include "camera.h"
#include "light.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(Texture);
SGD_SHARED(CollisionSpace);
SGD_SHARED(SceneRenderer);

SGD_SHARED(Scene);

struct Scene : Shared {
	SGD_OBJECT_TYPE(Scene, Shared);

	Signal<Vec2u> viewportSizeChanged;

	Scene();

	CVec2u viewportSize() {
		return m_viewportSize;
	}

	CVector<Camera*> cameras() const {
		return m_cameras;
	}

	void clear();
	void add(Entity* entity);
	void remove(Entity* entity);
	void validate();

	void render();

	CollisionSpace* collisionSpace() const {
		return m_collisionSpace;
	}

	SceneRenderer* sceneRenderer() const {
		return m_sceneRenderer;
	}

private:
	friend class Entity;

	Vec2u m_viewportSize;

	Vector<EntityPtr> m_entities;
	Vector<Camera*> m_cameras;
	Vector<Entity*> m_invalid;

	CollisionSpacePtr m_collisionSpace;
	SceneRendererPtr m_sceneRenderer;
};

} // namespace sgd
