#include "scenebindings.h"

#include "textureutil.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/scene.wgsl"
};

} // namespace

const BindGroupDescriptor sceneBindingsDescriptor( //
	"sceneBindings",							   //
	BindGroupType::scene,						   //

	{bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								wgpu::BufferBindingType::Uniform),		// binding(0) camera uniforms
	 bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment,			// binding(1) lighting uniforms
								wgpu::BufferBindingType::Uniform),		//
																		//
	 textureBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment,		// binding(2) lighting envTexture
								 wgpu::TextureViewDimension::Cube),		//
	 samplerBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment),		// binding(3) lighting envSampler
																		//
	 textureBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment,		// binding(4) directional light CSMTexture
								 wgpu::TextureViewDimension::e2DArray,	//
								 wgpu::TextureSampleType::Depth),		//
	 samplerBindGroupLayoutEntry(5, wgpu::ShaderStage::Fragment),		// binding(4) directional light CSMSampler
																		//
	 textureBindGroupLayoutEntry(6, wgpu::ShaderStage::Fragment,		// binding(4) point light PSMTexture
								 wgpu::TextureViewDimension::CubeArray, //
								 wgpu::TextureSampleType::Depth),		//
	 samplerBindGroupLayoutEntry(7, wgpu::ShaderStage::Fragment)},		// binding(4) point light PSMSampler
	{},																	//
	shaderSource);

SceneBindings::SceneBindings() {

	m_bindGroup = new BindGroup(&sceneBindingsDescriptor);

	CameraUniforms cameraUniforms;
	m_cameraUniforms = new Buffer(BufferType::uniform, &cameraUniforms, sizeof(cameraUniforms));
	m_bindGroup->setBuffer(0, m_cameraUniforms);

	LightingUniforms lightingUniforms;
	m_lightingUniforms = new Buffer(BufferType::uniform, &lightingUniforms, sizeof(lightingUniforms));
	m_bindGroup->setBuffer(1, m_lightingUniforms);

	setEnvTexture(whiteTexture(TextureFlags::cube));

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

	// Empty lighting uniforms
	LightingUniforms uniforms;
	m_shadowLightingUniforms = new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms));

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

void SceneBindings::setEnvTexture(CTexture* texture) {
	m_bindGroup->setTexture(2, texture);
}

void SceneBindings::updateCSMConfig() const {
	auto size = Vec2u(csmTextureSize());
	auto depth = maxCSMLights() * 4;
	if (!m_csmTexture || m_csmTexture->size() != size || m_csmTexture->depth() != depth) {
		m_csmTexture = new Texture(size, depth, TextureFormat::depth32f, //
								   TextureFlags::array | TextureFlags::renderTarget);
		m_bindGroup->setTexture(4, m_csmTexture);
	}
}

void SceneBindings::updatePSMConfig() const {
	auto size = Vec2u(psmTextureSize());
	auto depth = maxPSMLights() * 6;
	if (!m_psmTexture || m_psmTexture->size() != size || m_psmTexture->depth() != depth) {
		m_psmTexture = new Texture(size, depth, TextureFormat::depth32f, //
								   TextureFlags::cube | TextureFlags::array | TextureFlags::renderTarget);
		m_bindGroup->setTexture(6, m_psmTexture);
	}
	m_psmShadowPasses.clear();
	m_psmShadowPasses.resize(maxPSMLights() * 6);
	for (int i = 0; i < m_psmShadowPasses.size(); ++i) {
		auto cameraUniforms = new Buffer(BufferType::uniform, nullptr, sizeof(CameraUniforms));
		auto bindGroup = new BindGroup(&sceneBindingsDescriptor);
		bindGroup->setBuffer(0, cameraUniforms);
		bindGroup->setBuffer(1, m_shadowLightingUniforms);									  // dummy light uniforms
		bindGroup->setTexture(2, dummyTexture(TextureFormat::rgba8, TextureFlags::cube));	  // dummy env texture
		bindGroup->setTexture(4, dummyTexture(TextureFormat::depth32f, TextureFlags::array)); // dummy csm texture
		bindGroup->setTexture(
			6, dummyTexture(TextureFormat::depth32f, TextureFlags::cube | TextureFlags::array)); // dummy psm texture

		m_psmShadowPasses[i] = {new Texture(m_psmTexture, i), bindGroup};
	}
}

void SceneBindings::addPSMPasses() const {

	auto pass = m_psmShadowPasses.begin();

	for (int i = 0, n = 0; i < lightingUniforms().pointLightCount && n < maxPSMLights(); ++i) {

		auto& light = lightingUniforms().pointLights[i];
		if (!light.castsShadow) continue;
		++n;

		for (int face = 0; face < 6; ++face) {

			static constexpr Mat3f faceTransforms[]		//
				{{{0, 0, +1}, {0, +1, 0}, {-1, 0, 0}},	// 0: +X
				 {{0, 0, -1}, {0, +1, 0}, {+1, 0, 0}},	// 1: -X
				 {{+1, 0, 0}, {0, 0, +1}, {0, -1, 0}},	// 2: +Y
				 {{+1, 0, 0}, {0, 0, -1}, {0, +1, 0}},	// 3: -Y
				 {{+1, 0, 0}, {0, +1, 0}, {0, 0, +1}},	// 4: +Z
				 {{-1, 0, 0}, {0, +1, 0}, {0, 0, -1}}}; // 5: -Z

			float near = .1f;
			float far = light.range;

			auto projMatrix = Mat4f::frustum(-near, near, -near, near, near, far);
			auto faceMatrix = AffineMat4f(faceTransforms[face], {}) * inverse(AffineMat4f({}, light.position));

			CameraUniforms uniforms;
			uniforms.projectionMatrix = projMatrix;
			uniforms.worldMatrix = Mat4f(inverse(faceMatrix));
			uniforms.inverseProjectionMatrix = inverse(projMatrix);
			uniforms.viewMatrix = Mat4f(faceMatrix);
			uniforms.viewProjectionMatrix = uniforms.projectionMatrix * uniforms.viewMatrix;
			uniforms.clipNear = near;
			uniforms.clipFar = far;

			((Buffer*)pass->sceneBindings->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));

			m_shadowPasses.push_back(*pass);
			++pass;
		}
	}
}

void SceneBindings::onValidate(GraphicsContext* gc) const {
	if (m_configDirty) {
		updateCSMConfig();
		updatePSMConfig();
		m_configDirty = false;
	}
	if (m_passesDirty) {
		m_shadowPasses.clear();
		addPSMPasses();
		m_passesDirty = false;
	}
}

} // namespace sgd
