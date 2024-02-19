#include "pbrmaterial.h"

#include "shaders/uniforms.h"
#include "textureutil.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/pbrmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor(1,
										{
											bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment,
																	   wgpu::BufferBindingType::Uniform), // PBRMaterialUniforms
											textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),  // albedoTexture
											samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),  // albedoSampler
											textureBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment),  // emissiveTexture
											samplerBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment),  // emissiveSampler
											textureBindGroupLayoutEntry(5, wgpu::ShaderStage::Fragment),  // metallicTexture
											samplerBindGroupLayoutEntry(6, wgpu::ShaderStage::Fragment),  // metallicSampler
											textureBindGroupLayoutEntry(7, wgpu::ShaderStage::Fragment),  // roughnessTexture
											samplerBindGroupLayoutEntry(8, wgpu::ShaderStage::Fragment),  // roughnessSampler
											textureBindGroupLayoutEntry(9, wgpu::ShaderStage::Fragment),  // occlusionTexture
											samplerBindGroupLayoutEntry(10, wgpu::ShaderStage::Fragment), // occlusionSampler
											textureBindGroupLayoutEntry(11, wgpu::ShaderStage::Fragment), // normalTexture
											samplerBindGroupLayoutEntry(12, wgpu::ShaderStage::Fragment), // normalSampler
										},
										{}, shaderSource);
} // namespace

const MaterialDescriptor pbrMaterialDescriptor(
	"pbr",
	&bindGroupDescriptor,
	sizeof(PBRMaterialUniforms),
	{
		{"albedoColor4f", offsetof(PBRMaterialUniforms, albedoColor)},
		{"emissiveColor3f", offsetof(PBRMaterialUniforms, emissiveColor)},
		{"metallicFactor1f", offsetof(PBRMaterialUniforms, metallicFactor)},
		{"roughnessFactor1f", offsetof(PBRMaterialUniforms, roughnessFactor)},
	},
	{
		{"albedoTexture", 1},
		{"emissiveTexture", 3},
		{"metallicTexture", 5},
		{"roughnessTexture", 7},
		{"occlusionTexture", 9},
		{"normalTexture", 11},
	},
	[](Material* material) {
		material->setVector4f("albedoColor4f", Vec4f(1));
		material->setVector3f("emissiveColor3f", Vec3f(0));
		material->setFloat("metallicFactor1f", 0);
		material->setFloat("roughnessFactor1f", 1);
		material->setTexture("albedoTexture", whiteTexture());
		material->setTexture("emissiveTexture", whiteTexture());
		material->setTexture("metallicTexture", whiteTexture());
		material->setTexture("roughnessTexture", whiteTexture());
		material->setTexture("occlusionTexture", whiteTexture());
		material->setTexture("normalTexture", rgbaTexture(0xff008080));	// 0, 0, -1
	}
);

} // namespace sgd
