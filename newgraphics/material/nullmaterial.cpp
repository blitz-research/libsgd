#include "nullmaterial.h"

namespace sgd {

namespace {

auto shaderSource{
#include "nullmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"nullMaterial",						 //
	BindGroupType::material,			 //
	{},									 //
	shaderSource);

} // namespace

const MaterialDescriptor nullMaterialDescriptor( //
	"null",										 //
	&bindGroupDescriptor,						 //
	0,//
	{},											 //
	{});										 //

} // namespace sgd
