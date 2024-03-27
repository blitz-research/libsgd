#include "spritematerial.h"

#include "shaders/uniforms.h"
#include "textureutil.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/spritematerial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	1,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // SpriteMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),  // albedoSampler
	},
	{},
	shaderSource);
} // namespace

const MaterialDescriptor spriteMaterialDescriptor( //
	"pbr", //
	&bindGroupDescriptor, //
	sizeof(SpriteMaterialUniforms), //
	{
		{"albedoColor4f", {offsetof(SpriteMaterialUniforms, albedoColor), 4, new Vec4f(1)}},
	},
	{
		{"albedoTexture", {1, whiteTexture()}},
	}
);

} // namespace sgd
