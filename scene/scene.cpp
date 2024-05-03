#include "scene.h"

#include "camera.h"
#include "entity.h"
#include "light.h"
#include "collisionspace.h"

#include <window/exports.h>

namespace sgd {

namespace {

void setCameraUniforms(CameraUniforms& uniforms, CAffineMat4r worldMatrix, CMat4f projMatrix) {
	uniforms.worldMatrix.i = {worldMatrix.r.i, 0};
	uniforms.worldMatrix.j = {worldMatrix.r.j, 0};
	uniforms.worldMatrix.k = {worldMatrix.r.k, 0};
	uniforms.worldMatrix.t = {0, 0, 0, 1};
	uniforms.projectionMatrix = projMatrix;
	uniforms.viewMatrix = inverse(uniforms.worldMatrix);
	uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
	uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
}

} // namespace

Scene::Scene(GraphicsContext* gc) : m_gc(gc) {

	auto texture = new Texture({1, 1}, 6, TextureFormat::rgba8, TextureFlags::cube);
	uint32_t data[6]{0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000};
	texture->update(data, sizeof(uint32_t));
	envTexture = texture;

	m_sceneBindings = new SceneBindings();
	for (int i = 0; i < 6; ++i) {
		m_shadowBindings[i] = new SceneBindings();
	}

	m_renderContext = new RenderContext(m_gc);

	m_viewportSize = m_gc->window()->size();
	m_gc->window()->sizeChanged1.connect(this, [=](CVec2u size) {
		if (!m_gc->canRender()) return;
		SGD_ASSERT(size == m_gc->colorBuffer()->size());
		m_viewportSize = size;
		viewportSizeChanged.emit(size);
	});

	m_collisionSpace = new CollisionSpace();
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
	m_renderers = {};

	SGD_ASSERT(m_entities.empty());
}

void Scene::add(Entity* entity) { // NOLINT (recursive)
	SGD_ASSERT(!entity->m_scene);
	SGD_ASSERT(!entity->parent() || entity->parent()->scene() == this);

	m_entities.emplace_back(entity);

	if (entity->is<Camera>()) {
		auto camera = entity->as<Camera>();
		viewportSizeChanged.connect(camera, [=](CVec2u size){	//
			camera->aspect = (float)size.x / (float)size.y;
		});
		camera->aspect = (float)viewportSize().x / (float)viewportSize().y;
		m_cameras.emplace_back(camera);
	} else if (entity->is<Light>()) {
		auto light = entity->as<Light>();
		m_lights.emplace_back(light);
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

	if(entity->m_invalid) sgd::remove(m_invalid, entity);

	if (entity->is<Camera>()) {
		auto camera = entity->as<Camera>();
		viewportSizeChanged.disconnect(camera);
		sgd::remove(m_cameras, camera);
	} else if (entity->is<Light>()) {
		sgd::remove(m_lights, entity->as<Light>());
	}

	// Careful, this could be last reference and could delete entity
	if (!sgd::rremove(m_entities, entity)) SGD_PANIC("Failed to remove entity from scene");
}

void Scene::validate() {
	for (auto entity : m_invalid) entity->validate();
	m_invalid.clear();
}

void Scene::setRenderer(RendererType type, Renderer* renderer) {
	m_renderers[(uint32_t)type] = renderer;
}

Renderer* Scene::getRenderer(RendererType type) {
	return (uint32_t)type < 8 ? m_renderers[(uint32_t)type] : nullptr;
}

void Scene::updateCameraBindings() {
	CameraUniforms uniforms;
	if (m_cameras.empty()) {
		// Quick mouselook hack for no camera
		auto mouse = m_gc->window()->mouse()->position() / Vec2f(m_gc->window()->size()) * 2.0f - 1.0f;
		auto matrix = AffineMat4r::rotation({-mouse.y * halfPi, -mouse.x * pi, 0});
		auto aspect = (float)m_viewportSize.x / (float)m_viewportSize.y;
		auto proj = Mat4f::perspective(45, aspect, .1, 100);
		setCameraUniforms(uniforms, matrix, proj);
		m_eye = {};
	} else {
		auto camera = m_cameras.front();
		setCameraUniforms(uniforms, camera->worldMatrix(), camera->projectionMatrix());
		m_eye = camera->worldPosition();
	}
	m_sceneBindings->updateCameraUniforms(uniforms);
}

void Scene::updateLightingBindings() {

	LightingUniforms uniforms;
	uniforms.ambientLightColor = ambientLightColor();

	Vector<Light*> pointLights;

	uniforms.directionalLightCount = 0;
	uniforms.spotLightCount = 0;

	for (Light* light : m_lights) {
		if (!light->visible()) continue;
		switch (light->type()) {
		case LightType::undefined:
		case LightType::directional: {
			if (uniforms.directionalLightCount == LightingUniforms::maxDirectionalLights) break;
			auto& ulight = uniforms.directionalLights[uniforms.directionalLightCount++];
			ulight.direction = -light->worldMatrix().r.k;
			ulight.color = light->color();
			break;
		}
		case LightType::point: {
			pointLights.push_back(light);
			break;
		}
		case LightType::spot: {
			if (uniforms.spotLightCount == LightingUniforms::maxSpotLights) break;
			auto& ulight = uniforms.spotLights[uniforms.spotLightCount++];
			ulight.position = light->worldMatrix().t - m_eye;
			ulight.direction = -light->worldMatrix().r.k;
			ulight.color = light->color();
			ulight.range = light->range();
			ulight.falloff = light->falloff();
			ulight.innerConeAngle = light->innerConeAngle() * degreesToRadians;
			ulight.outerConeAngle = light->outerConeAngle() * degreesToRadians;
			break;
		}
		}
	}

	// sort point lights
	auto cmp = [=](const Light* lhs, const Light* rhs) {
		return length(rhs->worldMatrix().t - m_eye) > length(lhs->worldMatrix().t - m_eye);
	};
	std::sort(pointLights.begin(), pointLights.end(), cmp);

	uniforms.pointLightCount = std::min((int)pointLights.size(), LightingUniforms::maxPointLights);

	m_pointShadowLights.clear();

	for (int i = 0; i < uniforms.pointLightCount; ++i) {
		auto light = pointLights[i];
		auto& ulight = uniforms.pointLights[i];
		ulight.position = light->worldMatrix().t - m_eye;
		ulight.color = light->color();
		ulight.range = light->range();
		ulight.falloff = light->falloff();
		ulight.castsShadow = light->castsShadow();

		if (light->castsShadow()) m_pointShadowLights.push_back(light);
	}

	m_sceneBindings->updateLightingUniforms(uniforms);

	m_sceneBindings->setEnvTexture(envTexture());

	auto n = m_pointShadowLights.size();
	if (n && (!m_pointShadowTexture || n * 6 > m_pointShadowTexture->depth())) {
		m_pointShadowTexture = new Texture({1024, 1024}, n * 6, TextureFormat::depth32f,
										   TextureFlags::cube | TextureFlags::array | TextureFlags::renderTarget);
		m_pointShadowTextureFaces.resize(n * 6);
		for (int i = 0; i < n * 6; ++i) m_pointShadowTextureFaces[i] = new Texture(m_pointShadowTexture, i);

		m_sceneBindings->setPointShadowTexture(m_pointShadowTexture);
	}
}

void Scene::render() {

	if (!m_gc->canRender()) return;

	beginRender.emit();

	updateCameraBindings();

	updateLightingBindings();

	for (Renderer* r : m_renderers) {
		if (r && r->enabled()) r->onUpdate(m_eye);
	}

	runOnMainThread(
		[=] { // We should really add dependencies on these...
			GraphicsResource::validateAll(m_gc);
		},
		true);

	runOnMainThread(
		[=] { //
			renderASync();
		},
		isMainThread());
}

void Scene::renderPointLightShadowMaps() const {
	if (m_pointShadowLights.empty()) return;

	CameraUniforms cameraUniforms;

	for (int i = 0; i < m_pointShadowLights.size(); ++i) {

		auto light = m_pointShadowLights[i];

		for (int face = 0; face < 6; ++face) {

			static constexpr Mat3f faceTransforms[]		//
				{{{0, 0, +1}, {0, +1, 0}, {-1, 0, 0}},	// 0: +X
				 {{0, 0, -1}, {0, +1, 0}, {+1, 0, 0}},	// 1: -X
				 {{+1, 0, 0}, {0, 0, +1}, {0, -1, 0}},	// 2: +Y
				 {{+1, 0, 0}, {0, 0, -1}, {0, +1, 0}},	// 3: -Y
				 {{+1, 0, 0}, {0, +1, 0}, {0, 0, +1}},	// 4: +Z
				 {{-1, 0, 0}, {0, +1, 0}, {0, 0, -1}}}; // 5: -Z

			float near = .1f;
			float far = light->range();

			auto projMatrix = Mat4f::frustum(-near, near, -near, near, near, far);
			auto faceMatrix = AffineMat4f(faceTransforms[face], {}) * inverse(AffineMat4f({}, light->worldPosition() - m_eye));

			cameraUniforms.projectionMatrix = projMatrix;
			cameraUniforms.worldMatrix = Mat4f(inverse(faceMatrix));
			cameraUniforms.inverseProjectionMatrix = inverse(projMatrix);
			cameraUniforms.viewMatrix = Mat4f(faceMatrix);
			cameraUniforms.viewProjectionMatrix = cameraUniforms.projectionMatrix * cameraUniforms.viewMatrix;
			cameraUniforms.clipNear = near;
			cameraUniforms.clipFar = far;

			m_shadowBindings[face]->updateCameraUniforms(cameraUniforms);
		}

		// upload camera buffer changes
		GraphicsResource::validateAll(m_gc);

		m_renderContext->beginRender();

		for (int face = 0; face < 6; ++face) {

			auto texture = m_pointShadowTextureFaces[i * 6 + face];

			m_renderContext->beginRenderPass(RenderPassType::clear, nullptr, texture, {}, 1);
			m_renderContext->endRenderPass();

			m_renderContext->beginRenderPass(RenderPassType::shadow, nullptr, texture, {}, {});
			m_renderContext->wgpuRenderPassEncoder().SetBindGroup(0, m_shadowBindings[face]->bindGroup()->wgpuBindGroup());
			for (Renderer* r : m_renderers) {
				if (r && r->enabled()) r->render(m_renderContext);
			}
			m_renderContext->endRenderPass();
		}

		m_renderContext->endRender();
	}
}

void Scene::renderGeometry(RenderPassType renderPassType) const {
	m_renderContext->beginRenderPass(renderPassType, m_gc->colorBuffer(), m_gc->depthBuffer(), clearColor(), clearDepth());
	m_renderContext->wgpuRenderPassEncoder().SetBindGroup(0, m_sceneBindings->bindGroup()->wgpuBindGroup());
	for (Renderer* r : m_renderers) {
		if (r && r->enabled()) r->render(m_renderContext);
	}
	m_renderContext->endRenderPass();
}

void Scene::renderASync() const {

	renderPointLightShadowMaps();

	m_renderContext->beginRender();

	renderGeometry(RenderPassType::clear);

	renderGeometry(RenderPassType::opaque);

	renderGeometry(RenderPassType::blend);

	m_renderContext->endRender();
}

} // namespace sgd
