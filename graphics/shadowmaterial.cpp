#include "shadowmaterial.h"

#include "shaders/uniforms.h"
#include "textureutil.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/shadowmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"shadowMaterial",
	1,									 // material
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
