#include "nullmaterial.h"

namespace sgd {

namespace {

auto shaderSource{
#include "nullmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"nullMaterial",						 //
	BindGroupType::material,			 //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // NullMaterialUniforms
	},									 //
	shaderSource);

} // namespace

const MaterialDescriptor nullMaterialDescriptor( //
	"null",										 //
	&bindGroupDescriptor,						 //
	4,										     //
	{},											 //
	{});										 //

} // namespace sgd
