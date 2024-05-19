#include "scenebindings.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/scene.wgsl"
};

} // namespace

const BindGroupDescriptor sceneBindingsDescriptor( //
	"scneBindings",								   //
	0,											   //
	{bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								wgpu::BufferBindingType::Uniform),		// binding(0) camera uniforms
	 bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment,			// binding(1) lighting uniforms
								wgpu::BufferBindingType::Uniform),		//
	 textureBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment,		// binding(2) lighting envTexture
								 wgpu::TextureViewDimension::Cube),		//
	 samplerBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment),		// binding(3) lighting envSampler
	 textureBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment,		// binding(4) lighting shadowMapsTexture
								 wgpu::TextureViewDimension::CubeArray, //
								 wgpu::TextureSampleType::Depth),		//
	 samplerBindGroupLayoutEntry(5, wgpu::ShaderStage::Fragment)},		// binding(4) lighting shadowMapsSampler
	{},																	//
	shaderSource);

SceneBindings::SceneBindings() {

	CameraUniforms cameraUniforms{};
	m_cameraUniforms = new Buffer(BufferType::uniform, &cameraUniforms, sizeof(cameraUniforms));

	LightingUniforms lightingUniforms{};
	m_lightingUniforms = new Buffer(BufferType::uniform, &lightingUniforms, sizeof(lightingUniforms));

	auto envTexture = new Texture({1, 1}, 6, TextureFormat::rgba8, TextureFlags::cube);
	uint32_t data[6]{0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000};
	envTexture->update(data, sizeof(uint32_t));

	auto psTexture =
		new Texture({1, 1}, 6, TextureFormat::depth32f, TextureFlags::cube | TextureFlags::array | TextureFlags::renderTarget);

	m_bindGroup = new BindGroup(&sceneBindingsDescriptor);

	m_bindGroup->setBuffer(0, m_cameraUniforms);
	m_bindGroup->setBuffer(1, m_lightingUniforms);
	m_bindGroup->setTexture(2, envTexture);
	m_bindGroup->setTexture(4, psTexture);

	addDependency(m_bindGroup);
}

void SceneBindings::updateCameraUniforms(const CameraUniforms& uniforms) {
	m_cameraUniforms->update(&uniforms, 0, sizeof(uniforms));
}

void SceneBindings::updateLightingUniforms(const LightingUniforms& uniforms) {
	m_lightingUniforms->update(&uniforms, 0, sizeof(uniforms));
}

void SceneBindings::setEnvTexture(const Texture* texture) {
	m_bindGroup->setTexture(2, texture);
}

void SceneBindings::setPointShadowTexture(const Texture* texture) {
	m_bindGroup->setTexture(4, texture);
}

} // namespace sgd
