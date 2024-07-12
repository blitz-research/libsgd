#include "scenebindings.h"

#include "../core/textureutil.h"

namespace sgd {

namespace {

auto shaderSource{
#include "scene.wgsl"
};

BufferPtr g_dummyLightingUniforms;
BufferPtr g_dummyShadowUniforms;
BufferPtr g_dummyMatricesBuffer;

} // namespace

const BindGroupDescriptor sceneBindingsDescriptor( //
	"sceneBindings",							   //
	BindGroupType::scene,						   //

	{bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								wgpu::BufferBindingType::Uniform),		   // binding(0) camera uniforms
	 bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment,			   // binding(1) lighting uniforms
								wgpu::BufferBindingType::Uniform),		   //
	 bufferBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment,			   // binding(2) shadow uniforms
								wgpu::BufferBindingType::Uniform),		   //
																		   //
	 textureBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment,		   // binding(3) env texture_cube<f32>
								 wgpu::TextureViewDimension::Cube),		   //
	 samplerBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment),		   // binding(4) env sampler
																		   //
	 textureBindGroupLayoutEntry(5, wgpu::ShaderStage::Fragment,		   // binding(5) CSM texture_depth_2d_array
								 wgpu::TextureViewDimension::e2DArray,	   //
								 wgpu::TextureSampleType::Depth),		   //
	 samplerBindGroupLayoutEntry(6, wgpu::ShaderStage::Fragment,		   // binding(6) CSM sampler_comparison
								 wgpu::SamplerBindingType::Comparison),	   //
																		   //
	 bufferBindGroupLayoutEntry(7, wgpu::ShaderStage::Fragment,			   // binding(7) CSM array<mat4x4f>
								wgpu::BufferBindingType::ReadOnlyStorage), //
																		   //
	 textureBindGroupLayoutEntry(8, wgpu::ShaderStage::Fragment,		   // binding(8) PSM texture_depth_cube_array
								 wgpu::TextureViewDimension::CubeArray,	   //
								 wgpu::TextureSampleType::Depth),		   //
	 samplerBindGroupLayoutEntry(9, wgpu::ShaderStage::Fragment,		   //
								 wgpu::SamplerBindingType::Comparison)},   // binding(9) PSM sampler
	shaderSource);

SceneBindings::SceneBindings() {

	if (!g_dummyLightingUniforms) {
		LightingUniforms lightingUniforms;
		g_dummyLightingUniforms = new Buffer(BufferType::uniform, &lightingUniforms, sizeof(lightingUniforms));
		ShadowUniforms shadowUniforms;
		g_dummyShadowUniforms = new Buffer(BufferType::uniform, &shadowUniforms, sizeof(shadowUniforms));
		Mat4f matrices;
		g_dummyMatricesBuffer = new Buffer(BufferType::storage, &matrices, sizeof(matrices));
	}

	// Lighting
	m_bindGroup = new BindGroup(&sceneBindingsDescriptor);
	{
		CameraUniforms uniforms;
		m_cameraUniforms = new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms));
		m_bindGroup->setBuffer(0, m_cameraUniforms);
	}
	{
		LightingUniforms uniforms;
		m_lightingUniforms = new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms));
		m_bindGroup->setBuffer(1, m_lightingUniforms);
	}
	{
		m_shadowUniforms = new Buffer(BufferType::uniform, nullptr, sizeof(ShadowUniforms));
		m_bindGroup->setBuffer(2, m_shadowUniforms);
	}
	envTexture.changed.connect(nullptr, [=](CTexture* texture) { //
		m_bindGroup->setTexture(3, texture);
	});
	envTexture = blackTexture(TextureFlags::cube);

	// Config
	csmTextureSize.changed.connect(nullptr, [=](uint32_t size) { //
		m_configDirty = true;
		invalidate();
	});

	maxCSMLights.changed.connect(nullptr, [=](uint32_t max) { //
		m_configDirty = true;
		invalidate();
	});

	psmTextureSize.changed.connect(nullptr, [=](uint32_t size) { //
		m_configDirty = true;
		invalidate();
	});

	maxPSMLights.changed.connect(nullptr, [=](uint32_t max) { //
		m_configDirty = true;
		invalidate();
	});

	// Shadow uniforms
	csmSplitDistances.changed.connect(nullptr, [=](CArray<float, 4>) { //
		m_uniformsDirty = true;
		invalidate();
	});

	psmClipNear.changed.connect(nullptr, [=](float) { //
		m_uniformsDirty = true;
		invalidate();
	});

	csmDepthBias.changed.connect(nullptr, [=](float) {
		m_uniformsDirty = true;
		invalidate();
	});

	updateCSMConfig();
	updatePSMConfig();
}

void SceneBindings::setCameraUniforms(CCameraUniforms uniforms) {
	m_cameraUniforms->update(&uniforms, 0, sizeof(uniforms));
	m_passesDirty = true;
	invalidate();
}

void SceneBindings::setLightingUniforms(CLightingUniforms uniforms) {
	m_lightingUniforms->update(&uniforms, 0, sizeof(uniforms));
	m_passesDirty = true;
	invalidate();
}

void SceneBindings::setShadowUniforms(CShadowUniforms uniforms) {
	m_shadowUniforms->update(&uniforms, 0, sizeof(uniforms));
	m_passesDirty = true;
	invalidate();
}

BindGroup* SceneBindings::createShadowPassBindings() {
	auto bindGroup = new BindGroup(&sceneBindingsDescriptor);
	bindGroup->setBuffer(0, new Buffer(BufferType::uniform, nullptr, sizeof(CameraUniforms)));
	bindGroup->setBuffer(1, g_dummyLightingUniforms);
	bindGroup->setBuffer(2, g_dummyShadowUniforms);
	bindGroup->setTexture(3, dummyTexture(TextureFormat::rgba8, TextureFlags::cube));
	bindGroup->setTexture(5, dummyTexture(TextureFormat::depth32f, //
										  TextureFlags::filter | TextureFlags::array | TextureFlags::compare));
	bindGroup->setBuffer(7, g_dummyMatricesBuffer);
	bindGroup->setTexture(
		8, dummyTexture(TextureFormat::depth32f, //
						TextureFlags::filter | TextureFlags::cube | TextureFlags::array | TextureFlags::compare));
	return bindGroup;
}

void SceneBindings::updateCSMConfig() const {
	auto size = Vec2u(csmTextureSize());
	auto passes = maxCSMLights() * 4;
	if (!m_csmTexture || m_csmTexture->size() != size || m_csmTexture->depth() != passes) {
		m_csmTexture =
			new Texture(size, passes, TextureFormat::depth32f, //
						TextureFlags::filter | TextureFlags::array | TextureFlags::compare | TextureFlags::renderTarget);
		m_bindGroup->setTexture(5, m_csmTexture);
	}
	m_csmShadowPasses.clear();
	m_csmShadowPasses.resize(passes);
	for (int i = 0; i < passes; ++i) {
		auto bindGroup = createShadowPassBindings();
		m_csmShadowPasses[i] = {new Texture(m_csmTexture, i), bindGroup};
	}
	auto buffer = (Buffer*)m_bindGroup->getBuffer(7);
	if (!buffer) {
		buffer = new Buffer(BufferType::storage, nullptr, passes * sizeof(Mat4f));
		m_bindGroup->setBuffer(7, buffer);
	} else {
		buffer->resize(passes * sizeof(Mat4f));
	}
}

void SceneBindings::updatePSMConfig() const {
	auto size = Vec2u(psmTextureSize());
	auto passes = maxPSMLights() * 6;
	if (!m_psmTexture || m_psmTexture->size() != size || m_psmTexture->depth() != passes) {
		m_psmTexture = new Texture(size, passes, TextureFormat::depth32f, //
								   TextureFlags::filter | TextureFlags::cube | TextureFlags::array | TextureFlags::compare |
									   TextureFlags::renderTarget);
		m_bindGroup->setTexture(8, m_psmTexture);
	}
	m_psmShadowPasses.clear();
	m_psmShadowPasses.resize(passes);
	for (int i = 0; i < passes; ++i) {
		auto bindGroup = createShadowPassBindings();
		m_psmShadowPasses[i] = {new Texture(m_psmTexture, i), bindGroup};
	}
}

void SceneBindings::addCSMPasses() const {

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

	for (int i = 0, n = 0; i < lightingUniforms().directionalLightCount; ++i) {

		auto& light = lightingUniforms().directionalLights[i];
		if (!light.shadowsEnabled) continue;

		SGD_ASSERT(n++ < maxCSMLights());

		auto lightMatrix = AffineMat4f(light.worldMatrix);
		auto invLightMatrix = inverse(lightMatrix);
		auto boundsMatrix = invLightMatrix * worldMatrix;

		Boxf boundsNear;
		float dNear = cameraUniforms().clipNear;
		for (auto& ray : rays) boundsNear |= boundsMatrix * (ray * dNear);

		for (int split = 0; split < 4; ++split) {

			Boxf boundsFar;
			float dFar = csmSplitDistances()[split];
			for (auto& ray : rays) boundsFar |= boundsMatrix * (ray * dFar);

			auto bounds = boundsNear | boundsFar;

			bounds.min.z -= csmClipRange();

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

			((Buffer*)pass->sceneBindings->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));

			csmMatrices.push_back(uniforms.viewProjectionMatrix);

			m_shadowPasses.push_back(*pass++);
		}
	}
	((Buffer*)m_bindGroup->getBuffer(7))->update(csmMatrices.data(), 0, csmMatrices.size() * sizeof(Mat4f));
}

void SceneBindings::addPSMPasses() const {

	auto pass = m_psmShadowPasses.begin();

	for (int i = 0, n = 0; i < lightingUniforms().pointLightCount; ++i) {

		auto& light = lightingUniforms().pointLights[i];
		if (!light.shadowsEnabled) continue;

		SGD_ASSERT(n++ < maxPSMLights());

		auto lightMatrix = AffineMat4f({}, light.position);
		auto invLightMatrix = inverse(lightMatrix);

		for (int face = 0; face < 6; ++face) {

			static constexpr Mat3f faceTransforms[]		//
				{{{0, 0, +1}, {0, +1, 0}, {-1, 0, 0}},	// 0: +X
				 {{0, 0, -1}, {0, +1, 0}, {+1, 0, 0}},	// 1: -X
				 {{+1, 0, 0}, {0, 0, +1}, {0, -1, 0}},	// 2: +Y
				 {{+1, 0, 0}, {0, 0, -1}, {0, +1, 0}},	// 3: -Y
				 {{+1, 0, 0}, {0, +1, 0}, {0, 0, +1}},	// 4: +Z
				 {{-1, 0, 0}, {0, +1, 0}, {0, 0, -1}}}; // 5: -Z

			float near = psmClipNear();
			float far = light.range;

			auto projMatrix = Mat4f::frustum(-near, near, -near, near, near, far);

			auto biasMatrix = AffineMat4f::TRS({0, 0, 0});
			auto faceMatrix = AffineMat4f(faceTransforms[face], {}) * biasMatrix * invLightMatrix;

			CameraUniforms uniforms;
			uniforms.projectionMatrix = projMatrix;
			uniforms.worldMatrix = Mat4f(inverse(faceMatrix));
			uniforms.inverseProjectionMatrix = inverse(projMatrix);
			uniforms.viewMatrix = Mat4f(faceMatrix);
			uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
			uniforms.clipNear = near;
			uniforms.clipFar = far;

			((Buffer*)pass->sceneBindings->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));

			m_shadowPasses.push_back(*pass++);
		}
	}
}

void SceneBindings::onValidate() const {

	auto gc = currentGC();

	if (m_configDirty) {
		updateCSMConfig();
		updatePSMConfig();
		m_configDirty = false;
	}
	if (m_passesDirty) {
		m_shadowPasses.clear();
		addCSMPasses();
		addPSMPasses();
		m_passesDirty = false;
	}
	if (m_uniformsDirty) {
		ShadowUniforms uniforms;
		uniforms.csmSplitDistances = {csmSplitDistances()[0], csmSplitDistances()[1], csmSplitDistances()[2],
									  csmSplitDistances()[3]};
		uniforms.csmDepthBias = csmDepthBias();
		uniforms.psmClipNear = psmClipNear();
		uniforms.psmDepthBias = psmDepthBias();
		m_shadowUniforms->update(&uniforms, 0, sizeof(uniforms));
		m_uniformsDirty = false;
	}
}

} // namespace sgd
