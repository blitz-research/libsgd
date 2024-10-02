#include "terrainbindgroup.h"

namespace sgd {

namespace {

auto shaderSource{
#include "terrainrenderer.wgsl"
};

wgpu::VertexAttribute vertexAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0}, // Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // uint23_t RGBA
};
static_assert(sizeof(TerrainVertex) == 24);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(TerrainVertex), wgpu::VertexStepMode::Vertex, std::size(vertexAttribs),
												  vertexAttribs};

} // namespace

const BindGroupDescriptor terrainBindGroupDescriptor( //
	"terrainGeometry",								  //
	BindGroupType::geometry,						  //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
		textureBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment),
	},
	shaderSource,		  //
	{vertexBufferLayout}, //
	wgpu::PrimitiveTopology::TriangleList);
} // namespace sgd
