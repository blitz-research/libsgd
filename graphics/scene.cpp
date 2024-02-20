#include "scene.h"

#include "camera.h"
#include "entity.h"
#include "light.h"

#include "renderer.h"
#include "shaders/uniforms.h"

namespace sgd {

Scene::Scene(GraphicsContext* gc) : m_gc(gc) {
}

void Scene::add(Entity* entity) {
	SGD_ASSERT(!entity->m_scene);

	m_entities.push_back(entity);
	if (entity->is<Camera>()) {
		m_cameras.push_back(entity->as<Camera>());
	} else if (entity->is<Light>()) {
		m_lights.push_back(entity->as<Light>());
	}

	entity->m_scene = this;
	entity->onCreate();
	entity->setEnabled(true);
	entity->setVisible(true);
}

void Scene::remove(Entity* entity) {
	SGD_ASSERT(entity->m_scene == this);

	entity->setVisible(false);
	entity->setEnabled(false);
	entity->onDestroy();
	entity->m_scene = nullptr;

	sgd::remove(m_entities, entity);
	if (entity->is<Camera>()) {
		sgd::remove(m_cameras, entity->as<Camera>());
	} else if (entity->is<Light>()) {
		sgd::remove(m_lights, entity->as<Light>());
	}
}

Renderer* Scene::getRenderer(CObject* key) {
	auto it = m_renderers.find(key);
	return it != m_renderers.end() ? it->second : nullptr;
}

void Scene::addRenderer(CObject* key, Renderer* renderer) {
	log() << __FUNCTION__;
	SGD_ASSERT(m_renderers.find(key) == m_renderers.end());
	m_renderers.insert(std::make_pair(key, renderer));
}

void Scene::updateCameraUniforms() {

	CameraUniforms uniforms;

	auto aspect = (float)m_gc->colorBuffer()->size().x / (float)m_gc->colorBuffer()->size().y;

	if (m_cameras.empty()) {
		AffineMat4f matrix;

		uniforms.worldMatrix = Mat4f(matrix);
		uniforms.viewMatrix = Mat4f(inverse(matrix));

		uniforms.projectionMatrix = Mat4f::perspective(45, aspect, .1, 100);

		uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
		uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
	} else {
		auto camera = m_cameras.front();

		uniforms.worldMatrix = Mat4f(camera->worldMatrix());
		uniforms.viewMatrix = Mat4f(inverse(camera->worldMatrix()));

		uniforms.projectionMatrix = Mat4f::perspective(camera->fovY(), aspect, camera->near(), camera->far());
	}
	uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
	uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;

	m_gc->bindGroup0()->getBuffer(0)->update(&uniforms, 0, sizeof(uniforms));
}

void Scene::updateLightingUniforms() {

	LightingUniforms uniforms;
	uniforms.ambientLightColor = ambientLightColor();

	int i = 0;
	for (auto light : m_lights) {

		uniforms.pointLights[i].position = Vec4f(light->worldMatrix().t, 1);
		uniforms.pointLights[i].color = light->color();
		uniforms.pointLights[i].range = light->range();
		uniforms.pointLights[i].falloff = light->falloff();
		if (++i == uniforms.maxPointLights) break;
	}
	uniforms.pointLightCount = i;

	m_gc->bindGroup0()->getBuffer(1)->update(&uniforms, 0, sizeof(uniforms));
	m_gc->bindGroup0()->setTexture(2, envTexture());
}

void Scene::render() {

	updateCameraUniforms();

	updateLightingUniforms();

	for (auto it : m_renderers) it.second->onUpdate();

	GraphicsResource::validateAll(m_gc);

	m_gc->beginRender(clearColor(), clearDepth());

	for (auto pass : {RenderPass::clear, RenderPass::opaque, RenderPass::blend}) {

		m_gc->beginRenderPass(pass);

		for (auto it : m_renderers) it.second->render(m_gc);

		m_gc->endRenderPass();
	}

	m_gc->endRender();
}

} // namespace sgd
