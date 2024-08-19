#include "imagebindgroup.h"

namespace sgd {

const BindGroupDescriptor imageBindGroupDescriptor( //
	"imageGeometry",			   //
	BindGroupType::geometry,	   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform),
	},
	{},										//
	{},										//
	wgpu::PrimitiveTopology::TriangleList); //
}
