#include "planebindgroup.h"

namespace sgd {

namespace {

auto shaderSource {
#include "planerenderer.wgsl"
};

} // namespace

const BindGroupDescriptor planeBindGroupDescriptor( //
	"planeGeometry",						   //
	BindGroupType::geometry,				   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
	},
	shaderSource, //
	{},			  //
	wgpu::PrimitiveTopology::TriangleStrip);
} // namespace sgd
