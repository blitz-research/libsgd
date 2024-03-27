#include "mattematerial.h"

#include "shaders/uniforms.h"
#include "textureutil.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/mattematerial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	1,									 //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // MatteMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),								  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),								  // albedoSampler
	},
	{},	shaderSource);

} // namespace

const MaterialDescriptor matteMaterialDescriptor( //
	"matte",									  //
	&bindGroupDescriptor,						  //
	sizeof(MatteMaterialUniforms),				  //
	{
		{"albedoColor4f", {offsetof(MatteMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
	},
	{
		{"albedoTexture", {1, whiteTexture()}},
	});

} // namespace sgd
