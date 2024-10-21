#include "emissivematerial.h"

#include "../core/textureutil.h"

namespace sgd {

namespace {

struct alignas(16) EmissiveMaterialUniforms {
	Vec4f emissiveColor{1};
};

auto shaderSource{
#include "emissivematerial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"emissiveMaterial", BindGroupType::material,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment,
								   wgpu::BufferBindingType::Uniform), // EmissiveMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),  // albedoSampler
	},
	shaderSource);
} // namespace

const MaterialDescriptor emissiveMaterialDescriptor( //
	"emissive",										 //
	&bindGroupDescriptor,							 //
	sizeof(EmissiveMaterialUniforms),				 //
	{
		{"emissive", {offsetof(EmissiveMaterialUniforms, emissiveColor), 4, new Vec4f(1)}},
	},
	{
		{"emissive", {1, whiteTexture()}},
	},
	1);
} // namespace sgd
