#include "prelitmaterial.h"

#include "../core/textureutil.h"

namespace sgd {

namespace {

struct alignas(16) PrelitMaterialUniforms {
	Vec4f albedoColor{1};
};

auto shaderSource{
#include "prelitmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"prelitMaterial",
	BindGroupType::material,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // PrelitMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),								  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),								  // albedoSampler
	},
	shaderSource);
} // namespace

const MaterialDescriptor prelitMaterialDescriptor( //
	"prelit",										   //
	&bindGroupDescriptor,						   //
	sizeof(PrelitMaterialUniforms),				   //
	{
		{"albedo", {offsetof(PrelitMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
	},
	{
		{"albedo", {1, whiteTexture()}},
	},1);

} // namespace sgd
