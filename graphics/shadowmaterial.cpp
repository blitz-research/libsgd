#include "shadowmaterial.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/shadowmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"shadowMaterial",					 //
	BindGroupType::material,			 //
	{},									 //
	{},									 //
	shaderSource);

} // namespace

const BindGroup* shadowBindGroup() {
	static BindGroupPtr bindGroup;
	if (!bindGroup) bindGroup = new BindGroup(&bindGroupDescriptor);
	return bindGroup;
}

} // namespace sgd
