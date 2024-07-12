#include "scene.h"

#include "camera.h"
#include "light.h"
#include "scenerenderer.h"

#include "../physics/collisionspace.h"

#include <window/exports.h>

namespace sgd {

Scene::Scene() {

	auto gc = currentGC();

	m_viewportSize = gc->window()->size();

	gc->window()->sizeChanged1.connect(this, [=](CVec2u size) {
		auto gc = currentGC();
		if (!gc->canRender()) return;
		SGD_ASSERT(size == gc->colorBuffer()->size());
		m_viewportSize = size;
		viewportSizeChanged.emit(size);
	});

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

	if (entity->is<Camera>()) {
		auto camera = entity->as<Camera>();
		viewportSizeChanged.connect(camera, [=](CVec2u size) { //
			camera->viewportSize = size;
		});
		camera->viewportSize = viewportSize();
		m_cameras.emplace_back(camera);
	}

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

	if (entity->is<Camera>()) {
		auto camera = entity->as<Camera>();
		viewportSizeChanged.disconnect(camera);
		sgd::remove(m_cameras, camera);
	}

	// Careful, this could be last reference and could delete entity
	if (!sgd::rremove(m_entities, entity)) SGD_PANIC("Failed to remove entity from scene");
}

void Scene::validate() {
	for (auto entity : m_invalid) entity->validate();
	m_invalid.clear();
}

void Scene::render() {
	m_sceneRenderer->render(!m_cameras.empty() ? m_cameras.front() : nullptr);
}

} // namespace sgd
