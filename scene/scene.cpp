#include "scene.h"

#include "camera.h"
#include "collisionspace.h"
#include "entity.h"
#include "light.h"

#include <window/exports.h>

namespace sgd {

Scene::Scene(GraphicsContext* gc) : m_gc(gc) {

	//	auto texture = new Texture({1, 1}, 6, TextureFormat::rgba8, TextureFlags::cube);
	//	uint32_t data[6]{0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000};
	//	texture->update(data, sizeof(uint32_t));
	envTexture = whiteTexture(TextureFlags::cube);

	m_sceneBindings = new SceneBindings();

	m_renderContext = new RenderContext(m_gc);

	m_viewportSize = m_gc->window()->size();

	m_gc->window()->sizeChanged1.connect(this, [=](CVec2u size) {
		if (!m_gc->canRender()) return;
		SGD_ASSERT(size == m_gc->colorBuffer()->size());
		m_viewportSize = size;
		viewportSizeChanged.emit(size);
	});

	m_collisionSpace = new CollisionSpace();

	m_timeStampsEnabled = m_gc->wgpuDevice().GetAdapter().HasFeature(wgpu::FeatureName::TimestampQuery);
	if (m_timeStampsEnabled) {
		wgpu::QuerySetDescriptor qsDesc{};
		qsDesc.type = wgpu::QueryType::Timestamp;
		qsDesc.count = timeStampCount;
		m_timeStampQueries = gc->wgpuDevice().CreateQuerySet(&qsDesc);

		wgpu::BufferDescriptor bufDesc{};
		bufDesc.size = timeStampCount * 8;
		bufDesc.usage = wgpu::BufferUsage::QueryResolve | wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc;
		m_timeStampBuffer = gc->wgpuDevice().CreateBuffer(&bufDesc);

		bufDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
		m_timeStampResults = gc->wgpuDevice().CreateBuffer(&bufDesc);
	}
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
		viewportSizeChanged.connect(camera, [=](CVec2u size) { //
			camera->aspect = (float)size.x / (float)size.y;
		});
		camera->aspect = (float)viewportSize().x / (float)viewportSize().y;
		m_cameras.emplace_back(camera);
	} else if (entity->is<Light>()) {
		auto light = entity->as<Light>();
		switch (light->type()) {
		case LightType::directional:
			m_directionalLights.emplace_back(light);
			break;
		case LightType::point:
			m_pointLights.emplace_back(light);
			break;
		case LightType::spot:
			m_spotLights.emplace_back(light);
			break;
		}
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
	} else if (entity->is<Light>()) {
		auto light = entity->as<Light>();
		switch (light->type()) {
		case LightType::directional:
			sgd::remove(m_directionalLights, light);
			break;
		case LightType::point:
			sgd::remove(m_pointLights, light);
			break;
		case LightType::spot:
			sgd::remove(m_spotLights, light);
			break;
		}
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
	AffineMat4f worldMatrix;
	Mat4f projMatrix;
	if (m_cameras.empty()) {
		// Quick mouselook hack for no camera
		auto mouse = m_gc->window()->mouse()->position().xy() / Vec2f(m_gc->window()->size()) * 2.0f - 1.0f;
		worldMatrix = AffineMat4r::rotation({-mouse.y * halfPi, -mouse.x * pi, 0});
		auto aspect = (float)m_viewportSize.x / (float)m_viewportSize.y;
		projMatrix = Mat4f::perspective(45, aspect, .1, 100);
	} else {
		auto camera = m_cameras.front();
		worldMatrix = camera->worldMatrix();
		projMatrix = camera->projectionMatrix();
	}
	CameraUniforms uniforms;
	uniforms.worldMatrix.i = {worldMatrix.r.i, 0};
	uniforms.worldMatrix.j = {worldMatrix.r.j, 0};
	uniforms.worldMatrix.k = {worldMatrix.r.k, 0};
	uniforms.worldMatrix.t = {0, 0, 0, 1};
	uniforms.projectionMatrix = projMatrix;
	uniforms.viewMatrix = inverse(uniforms.worldMatrix);
	uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
	uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
	m_sceneBindings->setCameraUniforms(uniforms);
	m_eye = worldMatrix.t;
}

void Scene::updateLightingBindings() {

	LightingUniforms uniforms;
	uniforms.ambientLightColor = ambientLightColor();

	// Directional lights
	{
		auto cmp = [=](const Light* lhs, const Light* rhs) { return lhs->priority() > rhs->priority(); };
		std::sort(m_directionalLights.begin(), m_directionalLights.end(), cmp);

		uniforms.directionalLightCount = std::min((int)m_directionalLights.size(), LightingUniforms::maxDirectionalLights);
		for (int i = 0, n = 0; i < uniforms.directionalLightCount; ++i) {
			auto light = m_directionalLights[i];
			auto& ulight = uniforms.directionalLights[i];
			ulight.worldMatrix = light->worldMatrix();
			ulight.color = light->color();
			ulight.castsShadow = light->castsShadow() && n < m_sceneBindings->maxCSMLights();
			n += ulight.castsShadow;
		}
	}

	// Point lights
	{
		auto cmp = [=](const Light* lhs, const Light* rhs) {
			if (lhs->priority() != rhs->priority()) return lhs->priority() > rhs->priority();
			return lengthsq(lhs->worldMatrix().t - m_eye) < lengthsq(rhs->worldMatrix().t - m_eye);
		};
		std::sort(m_pointLights.begin(), m_pointLights.end(), cmp);

		uniforms.pointLightCount = std::min((int)m_pointLights.size(), LightingUniforms::maxPointLights);
		for (int i = 0, n = 0; i < uniforms.pointLightCount; ++i) {
			auto light = m_pointLights[i];
			auto& ulight = uniforms.pointLights[i];
			ulight.position = light->worldPosition() - m_eye;
			ulight.color = light->color();
			ulight.range = light->range();
			ulight.falloff = light->falloff();
			ulight.castsShadow = light->castsShadow() && n < m_sceneBindings->maxPSMLights();
			n += ulight.castsShadow;
		}
	}

	// Spot lights
	{
		auto cmp = [=](const Light* lhs, const Light* rhs) {
			if (lhs->priority() != rhs->priority()) return lhs->priority() > rhs->priority();
			return lengthsq(lhs->worldMatrix().t - m_eye) < lengthsq(rhs->worldMatrix().t - m_eye);
		};
		std::sort(m_spotLights.begin(), m_spotLights.end(), cmp);

		uniforms.spotLightCount = std::min((int)m_spotLights.size(), LightingUniforms::maxSpotLights);
		for (int i = 0; i < uniforms.spotLightCount; ++i) {
			auto light = m_spotLights[i];
			auto& ulight = uniforms.spotLights[i];
			ulight.position = light->worldPosition() - m_eye;
			ulight.color = light->color();
			ulight.range = light->range();
			ulight.falloff = light->falloff();
			ulight.innerConeAngle = light->innerConeAngle();
			ulight.outerConeAngle = light->outerConeAngle();
		}
	}

	m_sceneBindings->setLightingUniforms(uniforms);

	m_sceneBindings->setEnvTexture(envTexture());
}

void Scene::render() {

	if (!m_gc->canRender()) return;

	beginRender.emit();

	updateCameraBindings();

	updateLightingBindings();

	for (Renderer* r : m_renderers) {
		if (r && r->enabled()) r->onUpdate(m_eye);
	}

	runOnMainThread([=] { GraphicsResource::validateAll(m_gc); }, true);

	runOnMainThread(
		[=] { //
			renderASync();
		},
		isMainThread());
}

void Scene::renderASync() const {

	m_renderContext->beginRender();

	// Timestamp 0
	if (m_timeStampsEnabled) m_renderContext->wgpuCommandEncoder().WriteTimestamp(m_timeStampQueries, 0);

	{
		// Shadows
		for (auto& pass : m_sceneBindings->shadowPasses()) {
			m_renderContext->beginRenderPass(RenderPassType::shadow, nullptr, pass.renderTarget, {}, 1);
			m_renderContext->wgpuRenderPassEncoder().SetBindGroup(0, pass.sceneBindings->wgpuBindGroup());
			for (Renderer* r : m_renderers) {
				if (r && r->enabled()) r->render(m_renderContext);
			}
			m_renderContext->endRenderPass();
		}
	}

	// Timestamp 1
	if (m_timeStampsEnabled) m_renderContext->wgpuCommandEncoder().WriteTimestamp(m_timeStampQueries, 1);

	{
		// Opaque
		m_renderContext->beginRenderPass(RenderPassType::opaque, m_gc->colorBuffer(), m_gc->depthBuffer(), clearColor(),
										 clearDepth());
		m_renderContext->wgpuRenderPassEncoder().SetBindGroup(0, m_sceneBindings->bindGroup()->wgpuBindGroup());
		for (Renderer* r : m_renderers) {
			if (r) r->render(m_renderContext); //->render(r->renderOps(RenderPassType::opaque));
		}
		m_renderContext->endRenderPass();
	}

	// Timestamp 2
	if (m_timeStampsEnabled) m_renderContext->wgpuCommandEncoder().WriteTimestamp(m_timeStampQueries, 2);

	{
		// AlphaBlend
		m_renderContext->beginRenderPass(RenderPassType::blend, m_gc->colorBuffer(), m_gc->depthBuffer(), clearColor(),
										 clearDepth());
		m_renderContext->wgpuRenderPassEncoder().SetBindGroup(0, m_sceneBindings->bindGroup()->wgpuBindGroup());
		for (Renderer* r : m_renderers) {
			if (r) r->render(m_renderContext); //->render(r->renderOps(RenderPassType::blend));
		}
		m_renderContext->endRenderPass();
	}

	if (m_timeStampsEnabled) {

		// Timestamp 3
		m_renderContext->wgpuCommandEncoder().WriteTimestamp(m_timeStampQueries, 3);

		// Resolve queries
		m_renderContext->wgpuCommandEncoder().ResolveQuerySet(m_timeStampQueries, 0, 4, m_timeStampBuffer, 0);

		// Copy query results
		m_renderContext->wgpuCommandEncoder().CopyBufferToBuffer(m_timeStampBuffer, 0, m_timeStampResults, 0,
																 timeStampCount * 8);
	}

	m_renderContext->endRender();

	if (m_timeStampsEnabled) {

		m_timeStampsEnabled = false;

		m_timeStampResults.MapAsync(
			wgpu::MapMode::Read, 0, timeStampCount * 8,
			[](WGPUBufferMapAsyncStatus status, void* userdata) {
				if (status != WGPUBufferMapAsyncStatus_Success) {
					return;
				}
				auto scene = (Scene*)userdata;
				std::memcpy(scene->m_timeStamps, scene->m_timeStampResults.GetConstMappedRange(), timeStampCount * 8);
				scene->m_timeStampResults.Unmap();
				scene->m_timeStampsEnabled = true;
				auto elapsed = scene->m_timeStamps[3] - scene->m_timeStamps[1];
				scene->m_rps = (float)(1e9 / (double)elapsed);
			},
			(void*)this);
	}
}

} // namespace sgd
