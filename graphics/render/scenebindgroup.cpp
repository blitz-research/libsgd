#include "scenebindgroup.h"

namespace sgd {

namespace {

auto shaderSource{
#include "scene.wgsl"
};

} // namespace

const BindGroupDescriptor sceneBindingsDescriptor( //
	"sceneBindings",								  //
	BindGroupType::scene,									  //
	{
		bufferBindGroupLayoutEntry(configUniformsBinding, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		bufferBindGroupLayoutEntry(cameraUniformsBinding, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
		bufferBindGroupLayoutEntry(lightingUniformsBinding, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(envTextureCubeBinding, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
		textureBindGroupLayoutEntry(envTexture2DBinding, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(envSamplerBinding, wgpu::ShaderStage::Fragment),
		textureBindGroupLayoutEntry(csmTextureBinding, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2DArray,
									wgpu::TextureSampleType::Depth),
		samplerBindGroupLayoutEntry(csmSamplerBinding, wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::Comparison),
		bufferBindGroupLayoutEntry(csmMatricesBinding, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::ReadOnlyStorage),
		textureBindGroupLayoutEntry(psmTextureBinding, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::CubeArray,
									wgpu::TextureSampleType::Depth),
		samplerBindGroupLayoutEntry(psmSamplerBinding, wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::Comparison),
		textureBindGroupLayoutEntry(ssmTextureBinding, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2DArray,
									wgpu::TextureSampleType::Depth),
		samplerBindGroupLayoutEntry(ssmSamplerBinding, wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::Comparison),
		bufferBindGroupLayoutEntry(ssmMatricesBinding, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::ReadOnlyStorage),
	},
	String("#define SCENE_ENV_MAP_CUBE 1\n") + shaderSource);


const BindGroupDescriptor sceneShadowBindingsDescriptor( //
	"shadowBindings",									 //
	BindGroupType::scene,								 //
	{
		bufferBindGroupLayoutEntry(configUniformsBinding, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),

		bufferBindGroupLayoutEntry(cameraUniformsBinding, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
	},
	shaderSource);

} // namespace sgd
