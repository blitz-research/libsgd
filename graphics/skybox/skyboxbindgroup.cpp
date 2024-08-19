#include "skyboxbindgroup.h"

namespace sgd {

namespace {

auto shaderSource{
#include "skyboxrenderer.wgsl"
};

} // namespace

const BindGroupDescriptor skyboxBindGroupDescriptor( //
	"skyboxGeometry",						   //
	BindGroupType::geometry,				   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
		textureBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment),
	},
	shaderSource, //
	{},			  //
	wgpu::PrimitiveTopology::TriangleStrip);
} // namespace sgd
