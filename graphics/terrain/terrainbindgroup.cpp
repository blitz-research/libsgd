#include "terrainbindgroup.h"

namespace sgd {

namespace {

auto shaderSource{
#include "terrainrenderer.wgsl"
};

wgpu::VertexAttribute vertexAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0}, // Vec3f position
	{wgpu::VertexFormat::Unorm8x4, 12, 1}, // uint32_t RGBA
};
static_assert(sizeof(TerrainVertex) == 16);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(TerrainVertex),		//
												  wgpu::VertexStepMode::Vertex, //
												  std::size(vertexAttribs),		//
												  vertexAttribs};

} // namespace

const BindGroupDescriptor terrainBindGroupDescriptor( //
	"terrainGeometry",								  //
	BindGroupType::geometry,						  //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Vertex, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Vertex),
		textureBindGroupLayoutEntry(3, wgpu::ShaderStage::Vertex, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(4, wgpu::ShaderStage::Vertex),
	},
	shaderSource,		  //
	{vertexBufferLayout}, //
	wgpu::PrimitiveTopology::TriangleList);
} // namespace sgd
