#include "scenebindgroup.h"

#include <app/exports.h>

namespace sgd {

namespace {

auto shaderSource{
#include "scene.wgsl"
};

} // namespace

const BindGroupDescriptor sceneBindingsDescriptor( //
	"sceneBindings",							   //
	BindGroupType::scene,						   //
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

#define GET_INT_UNIFORM(UNAME, VNAME)                                                                                          \
	if (configVarExists(VNAME)) UNAME = std::stoi(getConfigVar(VNAME));
#define GET_FLOAT_UNIFORM(UNAME, VNAME)                                                                                        \
	if (configVarExists(VNAME)) UNAME = std::stof(getConfigVar(VNAME));

ConfigUniforms getConfigUniformsFromConfigVars() {

	ConfigUniforms uniforms;

	if (configVarExists("csm.splitDistances")) {
		auto str = getConfigVar("csm.splitDistances");
		auto bits = split(str, ",");
		if (bits.size() != 4) SGD_ERROR("Malformed csm.splitDistances Vec4f: \"" + str + "\"");
		uniforms.csmSplitDistances = {std::stof(bits[0]), std::stof(bits[1]), std::stof(bits[2]), std::stof(bits[3])};
	}
	GET_INT_UNIFORM(uniforms.csmTextureSize, "csm.textureSize");
	GET_INT_UNIFORM(uniforms.maxCSMLights, "csm.maxLights");
	GET_FLOAT_UNIFORM(uniforms.csmClipRange, "csm.clipRange");
	GET_FLOAT_UNIFORM(uniforms.csmDepthBias, "csm.depthBias");

	GET_INT_UNIFORM(uniforms.psmTextureSize, "psm.textureSize");
	GET_INT_UNIFORM(uniforms.maxPSMLights, "psm.maxLights");
	GET_FLOAT_UNIFORM(uniforms.psmClipNear, "psm.clipNear");
	GET_FLOAT_UNIFORM(uniforms.psmDepthBias, "psm.depthBias");

	GET_INT_UNIFORM(uniforms.ssmTextureSize, "ssm.textureSize");
	GET_INT_UNIFORM(uniforms.maxSSMLights, "ssm.maxLights");
	GET_FLOAT_UNIFORM(uniforms.ssmClipNear, "ssm.clipNear");
	GET_FLOAT_UNIFORM(uniforms.ssmDepthBias, "ssm.depthBias");

	return uniforms;
}

} // namespace sgd
