#include "scene.h"
#include "camera.h"
#include "entity.h"
#include "light.h"

#include "renderer.h"
#include "shaders/uniforms.h"

#include <window/exports.h>

namespace sgd {

Scene::Scene(GraphicsContext* gc) : m_gc(gc) {
	auto texture = new Texture({1, 1}, 6, TextureFormat::rgba8, TextureFlags::cube);
	uint32_t data[6]{0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000};
	texture->update(data, sizeof(uint32_t));
	envTexture = texture;
}

void Scene::add(Entity* entity) {
	SGD_ASSERT(!entity->m_scene);
	SGD_ASSERT(!entity->parent() || entity->parent()->scene() == this);

	m_entities.push_back(entity);
	if (entity->is<Camera>()) {
		m_cameras.push_back(entity->as<Camera>());
	} else if (entity->is<Light>()) {
		m_lights.push_back(entity->as<Light>());
	}

	entity->m_scene = this;
	entity->onCreate();
	if (entity->enabled()) entity->onEnable();
	if (entity->visible()) entity->onShow();

	for (auto child : entity->children()) add(child);
}

void Scene::remove(Entity* entity) {
	SGD_ASSERT(entity->m_scene == this);
	SGD_ASSERT(!entity->parent() || entity->parent()->scene() == this);

	for (auto child : entity->children()) remove(child);
	entity->m_children.clear();

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

void Scene::setRenderer(RendererType type, Renderer* renderer) {
	m_renderers[(uint32_t)type] = renderer;
}

Renderer* Scene::getRenderer(RendererType type) {
	return (uint32_t)type < 8 ? m_renderers[(uint32_t)type] : nullptr;
	;
}

void Scene::updateCameraUniforms(CameraUniforms& uniforms) const {

	auto aspect = (float)m_gc->colorBuffer()->size().x / (float)m_gc->colorBuffer()->size().y;

	if (m_cameras.empty()) {
		auto curs = m_gc->window()->mouse()->position() / Vec2f(m_gc->window()->size());

		curs = curs * Vec2f(twoPi, pi) - Vec2f(pi, halfPi);

		auto matrix = AffineMat4f::rotation({-curs.y, -curs.x, 0});

		uniforms.worldMatrix = Mat4f(matrix);
		uniforms.viewMatrix = Mat4f(inverse(matrix));

		uniforms.projectionMatrix = Mat4f::perspective(45, aspect, .1, 100);

		uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
		uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
	} else {
		auto camera = m_cameras.front();

		uniforms.worldMatrix = Mat4f(camera->worldMatrix());
		uniforms.viewMatrix = Mat4f(inverse(camera->worldMatrix()));

		uniforms.projectionMatrix = Mat4f::perspective(camera->fov(), aspect, camera->near(), camera->far());
	}
	uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
	uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
}

void Scene::updateLightingUniforms(LightingUniforms& uniforms) const {

	uniforms.ambientLightColor = ambientLightColor();

	for (auto light : m_lights) {
		if (!light->visible()) continue;

		switch (light->type()) {
		case LightType::directional: {
			if (uniforms.directionalLightCount == uniforms.maxDirectionalLights) break;
			auto& ulight = uniforms.directionalLights[uniforms.directionalLightCount++];
			ulight.direction = -light->worldMatrix().r.k;
			ulight.color = light->color();
			break;
		}
		case LightType::point: {
			if (uniforms.pointLightCount == uniforms.maxPointLights) break;
			auto& ulight = uniforms.pointLights[uniforms.pointLightCount++];
			ulight.position = light->worldMatrix().t;
			ulight.color = light->color();
			ulight.range = light->range();
			ulight.falloff = light->falloff();
			break;
		}
		case LightType::spot: {
			if (uniforms.spotLightCount == uniforms.maxSpotLights) break;
			auto& ulight = uniforms.spotLights[uniforms.spotLightCount++];
			ulight.position = light->worldMatrix().t;
			ulight.direction = -light->worldMatrix().r.k;
			ulight.color = light->color();
			ulight.range = light->range();
			ulight.falloff = light->falloff();
			ulight.innerConeAngle = light->innerConeAngle() * degreesToRadians;
			ulight.outerConeAngle = light->outerConeAngle() * degreesToRadians;
			break;
		}
		case LightType::undefined:
			SGD_ABORT();
		}
	}
}

void Scene::render() {

	CameraUniforms cameraUniforms;
	updateCameraUniforms(cameraUniforms);
	m_gc->bindGroup0()->getBuffer(0)->update(&cameraUniforms, 0, sizeof(cameraUniforms));

	LightingUniforms lightingUniforms;
	updateLightingUniforms(lightingUniforms);
	m_gc->bindGroup0()->getBuffer(1)->update(&lightingUniforms, 0, sizeof(lightingUniforms));
	m_gc->bindGroup0()->setTexture(2, envTexture());

	for (auto r : m_renderers) {
		if (r && r->enabled()) r->onUpdate(Vec3f(cameraUniforms.worldMatrix.t));
	}

	runOnMainThread(
		[=] { //
			GraphicsResource::validateAll(m_gc);
		},
		true);

	runOnMainThread(
		[=] { //
			renderASync();
		},
		isMainThread());
}

void Scene::renderASync() {
	m_gc->beginRender(clearColor(), clearDepth());

	for (auto pass : {RenderPass::clear, RenderPass::opaque, RenderPass::blend}) {

		m_gc->beginRenderPass(pass);

		for (auto r : m_renderers) {
			if (r && r->enabled()) r->render(m_gc);
		}

		m_gc->endRenderPass();
	}

	m_gc->endRender();
}

} // namespace sgd
