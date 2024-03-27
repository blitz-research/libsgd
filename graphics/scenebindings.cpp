#include "scenebindings.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/scene.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	0,									 //
	{bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								wgpu::BufferBindingType::Uniform), // binding(0) camera uniforms
	 bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment,
								wgpu::BufferBindingType::Uniform), // binding(1) lighting uniforms
	 textureBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment,
								 wgpu::TextureViewDimension::Cube), // binding(2) lighting envTexture
	 samplerBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment)},	// binding(3) lighting envSampler
	{},																//
	shaderSource);
} // namespace

SceneBindings::SceneBindings() {

	CameraUniforms cameraUniforms{};
	m_cameraUniforms = new Buffer(BufferType::uniform, &cameraUniforms, sizeof(cameraUniforms));

	LightingUniforms lightingUniforms{};
	m_lightingUniforms = new Buffer(BufferType::uniform, &lightingUniforms, sizeof(lightingUniforms));

	auto envTexture = new Texture({1, 1}, 6, TextureFormat::rgba8, TextureFlags::cube);
	uint32_t data[6]{0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000};
	envTexture->update(data, sizeof(uint32_t));

	m_bindGroup = new BindGroup(&bindGroupDescriptor);
	m_bindGroup->setBuffer(0, m_cameraUniforms);
	m_bindGroup->setBuffer(1, m_lightingUniforms);
	m_bindGroup->setTexture(2, envTexture);

	addDependency(m_bindGroup);
}

} // namespace sgd
