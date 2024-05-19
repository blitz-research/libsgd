#include "prelitmaterial.h"

#include "shaders/uniforms.h"
#include "textureutil.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/prelitmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"prelitMaterial",
	1,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // PrelitMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),								  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),								  // albedoSampler
	},
	{}, shaderSource);
} // namespace

const MaterialDescriptor prelitMaterialDescriptor( //
	"prelit",										   //
	&bindGroupDescriptor,						   //
	sizeof(PrelitMaterialUniforms),				   //
	{
		{"albedoColor4f", {offsetof(PrelitMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
	},
	{
		{"albedoTexture", {1, whiteTexture()}},
	});

CMaterialPtr defaultPrelitMaterial = new Material(&prelitMaterialDescriptor);

} // namespace sgd
