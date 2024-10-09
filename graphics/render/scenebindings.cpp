#include "scenebindings.h"

#include "../core/textureutil.h"

namespace sgd {

SceneBindings::SceneBindings()
	: m_bindGroup(new BindGroup(&sceneBindingsDescriptor)),								  //
	  m_configUniforms(new Buffer(BufferType::uniform, nullptr, sizeof(ConfigUniforms))), //
	  m_cameraUniforms(new Buffer(BufferType::uniform, nullptr, sizeof(CameraUniforms))), //
	  m_lightingUniforms(new Buffer(BufferType::uniform, nullptr, sizeof(LightingUniforms))),
	  envTexture(blackTexture(TextureFlags::cube)) { //

	lockConfigUniforms() = {};
	unlockConfigUniforms();

	lockCameraUniforms() = {};
	unlockCameraUniforms();

	lockLightingUniforms() = {};
	unlockLightingUniforms();

	m_bindGroup->setBuffer(configUniformsBinding, m_configUniforms);
	m_bindGroup->setBuffer(cameraUniformsBinding, m_cameraUniforms);
	m_bindGroup->setBuffer(lightingUniformsBinding, m_lightingUniforms);

	envTexture.changed.connect(nullptr, [=](CTexture* ctexture) { //
		if (!ctexture) ctexture = blackTexture(TextureFlags::cube);
		int type;
		if (bool(ctexture->flags() & TextureFlags::cube)) {
			m_bindGroup->setTexture(envTextureCubeBinding, ctexture, false);
			m_bindGroup->setTexture(envTexture2DBinding, blackTexture(), false);
			m_bindGroup->setSampler(envSamplerBinding, ctexture);
			type = 1;
		} else {
			m_bindGroup->setTexture(envTextureCubeBinding, blackTexture(TextureFlags::cube), false);
			m_bindGroup->setTexture(envTexture2DBinding, ctexture);
			type = 2;
		}
		if (type == lightingUniforms().envTextureType) return;
		lockLightingUniforms().envTextureType = type;
		unlockLightingUniforms();
	});
	envTexture.changed.emit(envTexture());

	updateCSMConfig();
	updatePSMConfig();
	updateSSMConfig();
}

BindGroup* SceneBindings::createShadowPassBindings() const {
	auto bindGroup = new BindGroup(&sceneShadowBindingsDescriptor);
	bindGroup->setBuffer(configUniformsBinding, m_configUniforms);
	bindGroup->setBuffer(cameraUniformsBinding, new Buffer(BufferType::uniform, nullptr, sizeof(CameraUniforms)));
	return bindGroup;
}

ConfigUniforms& SceneBindings::lockConfigUniforms() {
	return *(ConfigUniforms*)m_configUniforms->lock(0, sizeof(ConfigUniforms));
}

void SceneBindings::unlockConfigUniforms() {
	m_configUniforms->unlock();
	m_dirty |= Dirty::shadowConfigs | Dirty::shadowPasses;
	invalidate();
}

CameraUniforms& SceneBindings::lockCameraUniforms() {
	return *(CameraUniforms*)m_cameraUniforms->lock(0, sizeof(CameraUniforms));
}

void SceneBindings::unlockCameraUniforms() {
	m_cameraUniforms->unlock();
	m_dirty |= Dirty::shadowPasses;
	invalidate();
}

LightingUniforms& SceneBindings::lockLightingUniforms() {
	return *(LightingUniforms*)m_lightingUniforms->lock(0, sizeof(LightingUniforms));
}

void SceneBindings::unlockLightingUniforms() {
	m_lightingUniforms->unlock();
	m_dirty |= Dirty::shadowPasses;
	invalidate();
}

// ***** Directional light CSM shadows *****

void SceneBindings::updateCSMConfig() const {

	auto& config = configUniforms();

	auto size = Vec2u(config.csmTextureSize);
	auto passes = config.maxCSMLights * 4;

	if (!m_csmTexture || m_csmTexture->size() != size || m_csmTexture->depth() != passes) {
		m_csmTexture =
			new Texture(size, passes, TextureFormat::depth32f, //
						TextureFlags::filter | TextureFlags::clamp | TextureFlags::array | TextureFlags::renderTarget | TextureFlags::compare);
		m_bindGroup->setTexture(csmTextureBinding, m_csmTexture);
	}
	m_csmShadowPasses.clear();
	m_csmShadowPasses.resize(passes);
	for (int i = 0; i < passes; ++i) {
		auto bindGroup = createShadowPassBindings();
		m_csmShadowPasses[i] = {new Texture(m_csmTexture, i), bindGroup};
	}

	auto buffer = (Buffer*)m_bindGroup->getBuffer(csmMatricesBinding);
	if (!buffer) {
		buffer = new Buffer(BufferType::storage, nullptr, passes * sizeof(Mat4f));
		m_bindGroup->setBuffer(csmMatricesBinding, buffer);
	} else {
		buffer->resize(passes * sizeof(Mat4f));
	}
}

void SceneBindings::addCSMPasses() const {

	auto& config = configUniforms();
	auto& lighting = lightingUniforms();

	Vec3f rays[4];

	for (int i = 0; i < 4; ++i) {
		static constexpr Vec2f corners[]{{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
		auto clipCoords = cameraUniforms().inverseProjectionMatrix * Vec4f(corners[i], 0, 1);
		auto viewPos = clipCoords.xyz() / clipCoords.w;
		rays[i] = viewPos / viewPos.z;
	}

	auto worldMatrix = AffineMat4f(cameraUniforms().worldMatrix);

	auto pass = m_csmShadowPasses.begin();

	Vector<Mat4f> csmMatrices;

	for (int i = 0, n = 0; i < std::min(lighting.numCSMLights, config.maxCSMLights); ++i) {

		auto& light = lighting.directionalLights[i];

		auto lightMatrix = AffineMat4f(light.worldMatrix);
		auto invLightMatrix = inverse(lightMatrix);
		auto boundsMatrix = invLightMatrix * worldMatrix;

		Boxf boundsNear;
		float dNear = cameraUniforms().clipNear;
		for (auto& ray : rays) boundsNear |= boundsMatrix * (ray * dNear);

		for (int split = 0; split < 4; ++split) {

			Boxf boundsFar;
			float dFar = (&config.csmSplitDistances.x)[split];
			for (auto& ray : rays) boundsFar |= boundsMatrix * (ray * dFar);

			auto bounds = boundsNear | boundsFar;

			bounds.min.z -= config.csmClipRange;

			auto projMatrix = Mat4f::ortho(bounds.min.x, bounds.max.x, bounds.min.y, bounds.max.y, bounds.min.z, bounds.max.z);

			boundsNear = boundsFar;

			CameraUniforms uniforms;
			uniforms.projectionMatrix = projMatrix;
			uniforms.worldMatrix = lightMatrix;
			uniforms.inverseProjectionMatrix = inverse(uniforms.projectionMatrix);
			uniforms.viewMatrix = invLightMatrix;
			uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
			uniforms.clipNear = bounds.min.z;
			uniforms.clipFar = bounds.max.z;

			((Buffer*)pass->sceneBindings->getBuffer(cameraUniformsBinding))->update(&uniforms, 0, sizeof(uniforms));

			csmMatrices.push_back(uniforms.viewProjectionMatrix);

			m_shadowPasses.push_back(*pass++);
		}
	}
	((Buffer*)m_bindGroup->getBuffer(csmMatricesBinding))->update(csmMatrices.data(), 0, csmMatrices.size() * sizeof(Mat4f));
}

// ***** Point light PSM shadows *****

void SceneBindings::updatePSMConfig() const {

	auto& config = configUniforms();

	auto size = Vec2u(config.psmTextureSize);
	auto passes = config.maxPSMLights * 6;

	if (!m_psmTexture || m_psmTexture->size() != size || m_psmTexture->depth() != passes) {
		m_psmTexture = new Texture(size, passes, TextureFormat::depth32f,
								   TextureFlags::filter | TextureFlags::cube | TextureFlags::array |
									   TextureFlags::renderTarget | TextureFlags::compare);
		m_bindGroup->setTexture(psmTextureBinding, m_psmTexture);
	}
	m_psmShadowPasses.clear();
	m_psmShadowPasses.resize(passes);
	for (int i = 0; i < passes; ++i) {
		auto bindGroup = createShadowPassBindings();
		m_psmShadowPasses[i] = {new Texture(m_psmTexture, i), bindGroup};
	}
}

void SceneBindings::addPSMPasses() const {

	auto& config = configUniforms();
	auto& lighting = lightingUniforms();

	auto pass = m_psmShadowPasses.begin();

	for (int i = 0, n = 0; i < std::min(lighting.numPSMLights, config.maxPSMLights); ++i) {

		auto& light = lighting.pointLights[i];

		auto lightMatrix = AffineMat4f({}, light.position.xyz());
		auto invLightMatrix = inverse(lightMatrix);

		for (int face = 0; face < 6; ++face) {

			static constexpr Mat3f faceTransforms[]		//
				{{{0, 0, +1}, {0, +1, 0}, {-1, 0, 0}},	// 0: +X
				 {{0, 0, -1}, {0, +1, 0}, {+1, 0, 0}},	// 1: -X
				 {{+1, 0, 0}, {0, 0, +1}, {0, -1, 0}},	// 2: +Y
				 {{+1, 0, 0}, {0, 0, -1}, {0, +1, 0}},	// 3: -Y
				 {{+1, 0, 0}, {0, +1, 0}, {0, 0, +1}},	// 4: +Z
				 {{-1, 0, 0}, {0, +1, 0}, {0, 0, -1}}}; // 5: -Z

			float near = config.psmClipNear;
			float far = light.range;

			auto projMatrix = Mat4f::frustum(-near, near, -near, near, near, far);

			auto faceMatrix = AffineMat4f(faceTransforms[face], {}) * invLightMatrix;

			CameraUniforms uniforms;
			uniforms.projectionMatrix = projMatrix;
			uniforms.worldMatrix = inverse(faceMatrix);
			uniforms.inverseProjectionMatrix = inverse(projMatrix);
			uniforms.viewMatrix = faceMatrix;
			uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
			uniforms.clipNear = near;
			uniforms.clipFar = far;

			((Buffer*)pass->sceneBindings->getBuffer(cameraUniformsBinding))->update(&uniforms, 0, sizeof(uniforms));

			m_shadowPasses.push_back(*pass++);
		}
	}
}

// ***** Spot light SSM shadows *****

void SceneBindings::updateSSMConfig() const {

	auto& config = configUniforms();

	auto size = Vec2u(config.ssmTextureSize);
	auto passes = config.maxSSMLights;

	if (!m_ssmTexture || m_ssmTexture->size() != size || m_ssmTexture->depth() != passes) {
		m_ssmTexture =
			new Texture(size, passes, TextureFormat::depth32f, //
						TextureFlags::filter | TextureFlags::array | TextureFlags::renderTarget | TextureFlags::compare);
		m_bindGroup->setTexture(ssmTextureBinding, m_ssmTexture);
	}
	m_ssmShadowPasses.clear();
	m_ssmShadowPasses.resize(passes);
	for (int i = 0; i < passes; ++i) {
		auto bindGroup = createShadowPassBindings();
		m_ssmShadowPasses[i] = {new Texture(m_ssmTexture, i), bindGroup};
	}

	auto buffer = (Buffer*)m_bindGroup->getBuffer(ssmMatricesBinding);
	if (!buffer) {
		buffer = new Buffer(BufferType::storage, nullptr, passes * sizeof(Mat4f));
		m_bindGroup->setBuffer(ssmMatricesBinding, buffer);
	} else {
		buffer->resize(passes * sizeof(Mat4f));
	}
}

void SceneBindings::addSSMPasses() const {

	auto& config = configUniforms();
	auto& lighting = lightingUniforms();

	auto pass = m_ssmShadowPasses.begin();

	Vector<Mat4f> ssmMatrices;

	for (int i = 0, n = 0; i < std::min(lighting.numSSMLights, config.maxSSMLights); ++i) {

		auto& light = lighting.spotLights[i];

		float near = config.ssmClipNear;
		float far = light.range;
		auto projMatrix = Mat4f::frustum(-near, near, -near, near, near, far);

		CameraUniforms uniforms;
		uniforms.projectionMatrix = projMatrix;
		uniforms.worldMatrix = light.worldMatrix;
		uniforms.inverseProjectionMatrix = inverse(projMatrix);
		uniforms.viewMatrix = inverse(light.worldMatrix);
		uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
		uniforms.clipNear = near;
		uniforms.clipFar = far;

		((Buffer*)pass->sceneBindings->getBuffer(cameraUniformsBinding))->update(&uniforms, 0, sizeof(uniforms));

		m_shadowPasses.push_back(*pass++);
	}
	((Buffer*)m_bindGroup->getBuffer(ssmMatricesBinding))->update(ssmMatrices.data(), 0, ssmMatrices.size() * sizeof(Mat4f));
}

void SceneBindings::onValidate() const {
	if (bool(m_dirty & Dirty::shadowConfigs)) {
		updateCSMConfig();
		updatePSMConfig();
		updateSSMConfig();
	}
	if (bool(m_dirty & Dirty::shadowPasses)) {
		m_shadowPasses.clear();
		addCSMPasses();
		addPSMPasses();
		addSSMPasses();
	}
	m_dirty = {};
}

} // namespace sgd
