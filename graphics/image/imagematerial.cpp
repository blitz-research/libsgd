#include "imagematerial.h"

#include "../core/textureutil.h"

namespace sgd {

namespace {

struct alignas(16) ImageMaterialUniforms {
	Vec4f albedoColor;
};

auto shaderSource{
#include "imagematerial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"imageMaterial",					 //
	BindGroupType::material,			 //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
	}, //
	shaderSource);

BindGroupDescriptor bindGroupDescriptorMX( //
	"imageMaterialMX",					   //
	BindGroupType::material,			   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2DArray),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
	}, //
	String("#define IMAGE_MATERIAL_MX 1\n") + shaderSource);

TexturePtr g_defaultTextureMX = new Texture({1, 1}, 1, TextureFormat::r8, TextureFlags::array | TextureFlags::clamp);

} // namespace

const MaterialDescriptor imageMaterialDescriptor( //
	"image",									  //
	&bindGroupDescriptor,						  //
	sizeof(ImageMaterialUniforms),				  //
	{
		{"albedoColor4f", {offsetof(ImageMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
	},
	{
		{"albedoTexture", {1, whiteTexture()}},

	},
	1); //

const MaterialDescriptor imageMaterialDescriptorMX( //
	"image",										//
	&bindGroupDescriptorMX,							//
	sizeof(ImageMaterialUniforms),					//
	{
		{"albedoColor4f", {offsetof(ImageMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
	},
	{
		{"albedoTexture", {1, g_defaultTextureMX}},

	},
	1); //

} // namespace sgd
