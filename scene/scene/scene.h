#pragma once

#include "camera.h"
#include "light.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(SceneRenderer);
SGD_SHARED(CollisionSpace);
SGD_SHARED(AudioSpace);

SGD_SHARED(Scene);

struct Scene : Shared {
	SGD_OBJECT_TYPE(Scene, Shared);

	Scene();

	void clear();
	void add(Entity* entity);
	void remove(Entity* entity);
	void validate();

	void render();

	SceneRenderer* sceneRenderer() const {
		return m_sceneRenderer;
	}

	CollisionSpace* collisionSpace() const {
		return m_collisionSpace;
	}

	AudioSpace* audioSpace()const{
		return m_audioSpace;
	}

private:
	friend class Entity;

	Vector<EntityPtr> m_entities;

	Vector<Entity*> m_invalid;

	CollisionSpacePtr m_collisionSpace;
	SceneRendererPtr m_sceneRenderer;
	AudioSpacePtr m_audioSpace;
};

} // namespace sgd
