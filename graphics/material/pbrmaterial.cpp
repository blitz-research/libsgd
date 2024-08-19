#include "pbrmaterial.h"

#include "../core/textureutil.h"

namespace sgd {

namespace {

struct alignas(16) PBRMaterialUniforms {
	Vec4f albedoColor;
	Vec3f emissiveColor;
	float metallicFactor{};
	float roughnessFactor{};
};

auto shaderSource{
#include "pbrmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"pbrMaterial", BindGroupType::material,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // PBRMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),								  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),								  // albedoSampler
		textureBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment),								  // emissiveTexture
		samplerBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment),								  // emissiveSampler
		textureBindGroupLayoutEntry(5, wgpu::ShaderStage::Fragment),								  // metallicTexture
		samplerBindGroupLayoutEntry(6, wgpu::ShaderStage::Fragment),								  // metallicSampler
		textureBindGroupLayoutEntry(7, wgpu::ShaderStage::Fragment),								  // roughnessTexture
		samplerBindGroupLayoutEntry(8, wgpu::ShaderStage::Fragment),								  // roughnessSampler
		textureBindGroupLayoutEntry(9, wgpu::ShaderStage::Fragment),								  // occlusionTexture
		samplerBindGroupLayoutEntry(10, wgpu::ShaderStage::Fragment),								  // occlusionSampler
		textureBindGroupLayoutEntry(11, wgpu::ShaderStage::Fragment),								  // normalTexture
		samplerBindGroupLayoutEntry(12, wgpu::ShaderStage::Fragment),								  // normalSampler
	},
	shaderSource);

} // namespace

const MaterialDescriptor pbrMaterialDescriptor( //
	"pbr",										//
	&bindGroupDescriptor,						//
	sizeof(PBRMaterialUniforms),
	{
		{"albedoColor4f", {offsetof(PBRMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
		{"emissiveColor3f", {offsetof(PBRMaterialUniforms, emissiveColor), 3, new Vec3f(0)}},
		{"metallicFactor1f", {offsetof(PBRMaterialUniforms, metallicFactor), 1, new float(0)}},
		{"roughnessFactor1f", {offsetof(PBRMaterialUniforms, roughnessFactor), 1, new float(1)}},
	},
	{
		{"albedoTexture", {1, whiteTexture()}},
		{"emissiveTexture", {3, whiteTexture()}},
		{"metallicTexture", {5, whiteTexture()}},
		{"roughnessTexture", {7, whiteTexture()}},
		{"occlusionTexture", {9, whiteTexture()}},
		{"normalTexture", {11, flatTexture()}},
	},
	1);

} // namespace sgd
