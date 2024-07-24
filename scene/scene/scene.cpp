#include "scene.h"

#include "scenerenderer.h"

#include "../physics/collisionspace.h"

namespace sgd {

Scene::Scene() {

	m_collisionSpace = new CollisionSpace();

	m_sceneRenderer = new SceneRenderer();
}

void Scene::clear() {
	Vector<Entity*> roots;
	for (Entity* entity : m_entities) {
		if (!entity->parent()) roots.push_back(entity);
	}
	while (!roots.empty()) {
		remove(roots.back());
		roots.pop_back();
	}
	SGD_ASSERT(m_entities.empty());
}

void Scene::add(Entity* entity) { // NOLINT (recursive)
	SGD_ASSERT(!entity->m_scene);
	SGD_ASSERT(!entity->parent() || entity->parent()->scene() == this);

	m_entities.emplace_back(entity);

	entity->create(this);

	for (Entity* child : entity->children()) add(child);
}

void Scene::remove(Entity* entity) { // NOLINT (recursive)
	SGD_ASSERT(entity->m_scene == this);
	SGD_ASSERT(!entity->parent() || entity->parent()->scene() == this);

	while (!entity->children().empty()) {
		remove(entity->children().back());
	}

	entity->setParent(nullptr);
	entity->destroy();

	if (entity->m_invalid) sgd::remove(m_invalid, entity);

	// Careful, this could be last reference and could delete entity
	if (!sgd::rremove(m_entities, entity)) SGD_PANIC("Failed to remove entity from scene");
}

void Scene::validate() {
	for (auto entity : m_invalid) entity->validate();
	m_invalid.clear();
}

void Scene::render() {
	m_sceneRenderer->render();
}

} // namespace sgd
