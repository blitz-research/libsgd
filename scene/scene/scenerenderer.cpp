#include "scenerenderer.h"

#include "../model/modelrenderer.h"
#include "../model/skinnedmodelrenderer.h"
#include "../overlay/overlayrenderer.h"
#include "../skybox/skyboxrenderer.h"
#include "../sprite/spriterenderer.h"

namespace sgd {

SceneRenderer::SceneRenderer()
	: m_sceneBindings(new SceneBindings()),				  //
	  m_renderContext(new RenderContext()),				  //
	  m_renderQueue(new RenderQueue()),					  //
	  m_skyboxRenderer(new SkyboxRenderer()),			  //
	  m_modelRenderer(new ModelRenderer()),				  //
	  m_skinnedModelRenderer(new SkinnedModelRenderer()), //
	  m_spriteRenderer(new SpriteRenderer()),			  //
	  m_overlayRenderer(new OverlayRenderer()),			  //
	  envTexture(blackTexture(TextureFlags::cube)) {	  //

	auto gc = currentGC();
	m_timeStampsEnabled = gc->wgpuDevice().GetAdapter().HasFeature(wgpu::FeatureName::TimestampQuery);
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

void SceneRenderer::add(CLight* light) {
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

void SceneRenderer::remove(CLight* light) {
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

void SceneRenderer::updateCameraUniforms() {
	AffineMat4f worldMatrix;
	Mat4f projMatrix;
	if (!m_camera) {
		auto window = currentGC()->window();
		// Quick mouselook hack for no camera
		auto mouse = window->mouse()->position().xy() / Vec2f(window->size()) * 2.0f - 1.0f;
		worldMatrix = AffineMat4r::rotation({-mouse.y * halfPi, -mouse.x * pi, 0});
		auto aspect = (float)window->size().x / (float)window->size().y;
		projMatrix = Mat4f::perspective(45, aspect, .1, 100);
	} else {
		worldMatrix = m_camera->worldMatrix();
		projMatrix = m_camera->projectionMatrix();
	}
	auto& uniforms = m_sceneBindings->lockCameraUniforms();
	uniforms.worldMatrix.i = {worldMatrix.r.i, 0};
	uniforms.worldMatrix.j = {worldMatrix.r.j, 0};
	uniforms.worldMatrix.k = {worldMatrix.r.k, 0};
	uniforms.worldMatrix.t = {0, 0, 0, 1};
	uniforms.projectionMatrix = projMatrix;
	uniforms.viewMatrix = inverse(uniforms.worldMatrix);
	uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
	uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
	m_sceneBindings->unlockCameraUniforms();
}

void SceneRenderer::updateLightingUniforms() {

	auto& config = m_sceneBindings->configUniforms();
	auto& uniforms = m_sceneBindings->lockLightingUniforms();

	uniforms.ambientLightColor = ambientLightColor();

	// ***** Directional lights *****
	{
		auto& lights = m_directionalLights;
		auto cmp = [=](const Light* lhs, const Light* rhs) { //
			return lhs->priority() > rhs->priority();
		};
		std::sort(lights.begin(), lights.end(), cmp);
		uniforms.numDirectionalLights = std::min((int)lights.size(), maxDirectionalLights);
		int n = 0;
		for (bool shadow : {true, false}) {
			uniforms.numCSMLights = n;
			for (int i = 0; i < uniforms.numDirectionalLights; ++i) {
				auto light = lights[i];
				if (light->shadowsEnabled() != shadow) continue;
				auto& ulight = uniforms.directionalLights[n++];
				ulight.worldMatrix = light->worldMatrix();
				ulight.color = light->color();
			}
		}
		SGD_ASSERT(uniforms.numDirectionalLights == n);
	}

	// ***** Point lights *****
	{
		auto& lights = m_pointLights;
		auto cmp = [=](const Light* lhs, const Light* rhs) {
			if (lhs->priority() != rhs->priority()) return lhs->priority() > rhs->priority();
			return lengthsq(lhs->worldMatrix().t - m_eye) < lengthsq(rhs->worldMatrix().t - m_eye);
		};
		std::sort(lights.begin(), lights.end(), cmp);
		uniforms.numPointLights = std::min((int)lights.size(), maxPointLights);
		int n = 0;
		for (bool shadow : {true, false}) {
			uniforms.numPSMLights = n;
			for (int i = 0; i < uniforms.numDirectionalLights; ++i) {
				auto light = lights[i];
				if (light->shadowsEnabled() != shadow) continue;
				auto& ulight = uniforms.pointLights[n++];
				ulight.position = Vec4f(light->worldPosition() - m_eye, 1);
				ulight.color = light->color();
				ulight.range = light->range();
				ulight.falloff = light->falloff();
			}
		}
		SGD_ASSERT(uniforms.numPointLights == n);
	}

	// ***** Spot lights *****
	{
		auto& lights = m_spotLights;
		auto cmp = [=](const Light* lhs, const Light* rhs) {
			if (lhs->priority() != rhs->priority()) return lhs->priority() > rhs->priority();
			return lengthsq(lhs->worldMatrix().t - m_eye) < lengthsq(rhs->worldMatrix().t - m_eye);
		};
		std::sort(lights.begin(), lights.end(), cmp);
		uniforms.numSpotLights = std::min((int)lights.size(), maxSpotLights);
		int n = 0;
		for (bool shadow : {true, false}) {
			uniforms.numSSMLights = n;
			for (int i = 0; i < uniforms.numDirectionalLights; ++i) {
				auto light = lights[i];
				if (light->shadowsEnabled() != shadow) continue;
				auto& ulight = uniforms.spotLights[n++];
				ulight.position = Vec4f(light->worldPosition() - m_eye, 1);
				ulight.color = light->color();
				ulight.range = light->range();
				ulight.falloff = light->falloff();
				ulight.innerConeAngle = light->innerConeAngle();
				ulight.outerConeAngle = light->outerConeAngle();
			}
		}
		SGD_ASSERT(uniforms.numSpotLights == n);
	}
	m_sceneBindings->unlockLightingUniforms();
}

void SceneRenderer::render(CCamera* camera) {

	m_camera = camera;
	m_eye = camera ? camera->worldPosition() : Vec3r();

	m_renderQueue->clear();

	updateCameraUniforms();
	updateLightingUniforms();
	m_sceneBindings->envTexture = envTexture();

	m_skyboxRenderer->update(m_eye);
	m_modelRenderer->update(m_eye);
	m_skinnedModelRenderer->update(m_eye);
	m_spriteRenderer->update(m_eye);
	m_overlayRenderer->update(m_eye);

	runOnMainThread(
		[=] { //
			GraphicsResource::validateAll();
			m_skyboxRenderer->render(m_renderQueue);
			m_modelRenderer->render(m_renderQueue);
			m_skinnedModelRenderer->render(m_renderQueue);
			m_spriteRenderer->render(m_renderQueue);
			m_overlayRenderer->render(m_renderQueue);
		},
		true);

	runOnMainThread(
		[=] { //
			renderAsync();
		},
		isMainThread());
}

// ***** Async rendering *****

void SceneRenderer::renderGeometry(RenderPassType rpassType, Texture* colorBuffer, Texture* depthBuffer, CVec4f clearColor,
								   float clearDepth, BindGroup* sceneBindings) {

	m_renderContext->beginRenderPass(rpassType, colorBuffer, depthBuffer, clearColor, clearDepth,
									 sceneBindings->wgpuBindGroup());

	m_renderContext->render(m_renderQueue->renderOps(rpassType));

	m_renderContext->endRenderPass();
}

void SceneRenderer::renderAsync() {

	auto gc = currentGC();

	m_wgpuCommandEncoder = m_renderContext->beginRender();

	auto timeStampsEnabled = m_timeStampsEnabled;
	m_timeStampsEnabled = false;

	// Shadows
	if (timeStampsEnabled) m_wgpuCommandEncoder.WriteTimestamp(m_timeStampQueries, 0);
	for (auto& pass : m_sceneBindings->shadowPasses()) {
		renderGeometry(RenderPassType::shadow, nullptr, pass.renderTarget, {}, 1, pass.sceneBindings);
	}

	// Opaque
	if (timeStampsEnabled) m_wgpuCommandEncoder.WriteTimestamp(m_timeStampQueries, 1);
	renderGeometry(RenderPassType::opaque, gc->colorBuffer(), gc->depthBuffer(), clearColor(), clearDepth(),
				   m_sceneBindings->bindGroup());

	// Blend
	if (timeStampsEnabled) m_wgpuCommandEncoder.WriteTimestamp(m_timeStampQueries, 2);
	renderGeometry(RenderPassType::blend, gc->colorBuffer(), gc->depthBuffer(), clearColor(), clearDepth(),
				   m_sceneBindings->bindGroup());
	if (timeStampsEnabled) {
		m_wgpuCommandEncoder.WriteTimestamp(m_timeStampQueries, 3);
		m_wgpuCommandEncoder.ResolveQuerySet(m_timeStampQueries, 0, 4, m_timeStampBuffer, 0);
		m_wgpuCommandEncoder.CopyBufferToBuffer(m_timeStampBuffer, 0, m_timeStampResults, 0, timeStampCount * 8);
	}

	m_renderContext->endRender();

	if (timeStampsEnabled) {
		m_timeStampResults.MapAsync(								   //
			wgpu::MapMode::Read, 0, timeStampCount * 8,				   //
			wgpu::CallbackMode::AllowProcessEvents,					   //
			[this](wgpu::MapAsyncStatus status, char const* message) { //
				if (status != wgpu::MapAsyncStatus::Success) {
					return;
				}
				auto scene = (SceneRenderer*)this;
				std::memcpy(scene->m_timeStamps, scene->m_timeStampResults.GetConstMappedRange(), timeStampCount * 8);
				scene->m_timeStampResults.Unmap();
				auto elapsed = scene->m_timeStamps[3] - scene->m_timeStamps[0];
				scene->m_rps = (float)(1e9 / (double)elapsed);
				scene->m_timeStampsEnabled = true;
			});
	}
}

} // namespace sgd
