#include "meshrenderer.h"

#include "../render/renderqueue.h"

namespace sgd {

namespace {

auto shaderSource =
#include "meshrenderer.wgsl"
	;

wgpu::VertexAttribute vertexAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x3, 40, 3}, // Vec3f texCoords
	{wgpu::VertexFormat::Float32x4, 52, 4}, // Vec4f color
											// uchar8_t joints[4];
											// float weights[4];
};
static_assert(sizeof(Vertex) == 88);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexAttribs),
												  vertexAttribs};
BindGroupDescriptor bindGroupDescriptor( //
	"meshRenderer",						 //
	BindGroupType::renderer,			 //
	{
		// renderer_instances: MeshInstance[]
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::ReadOnlyStorage),
	},
	shaderSource,							//
	{vertexBufferLayout},					//
	wgpu::PrimitiveTopology::TriangleList); //

} // namespace

MeshRenderer::MeshRenderer()							//
	: m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_instanceBuffer(new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(MeshInstance))) {

	m_bindGroup->setBuffer(0, m_instanceBuffer);
}

MeshInstance* MeshRenderer::lockInstances(uint32_t count) {
	m_instanceCount = count;
	if (m_instanceCount > m_instanceCapacity) {
		m_instanceCapacity = m_instanceCount;
		m_instanceBuffer->resize(m_instanceCapacity * sizeof(MeshInstance));
	}
	return (MeshInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(MeshInstance));
}

void MeshRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}

void MeshRenderer::render(RenderQueue* rq, CMesh* mesh, uint32_t count, uint32_t first) const {
	for (Surface* surf : mesh->surfaces()) {
		rq->addRenderOp(mesh->vertexBuffer(), surf->triangleBuffer(),	  //
						surf->material(), mesh->bindGroup(), m_bindGroup, //
						surf->triangleCount() * 3, count, 0, first, mesh->shadowsEnabled());
	}
}

void MeshRenderer::render(RenderQueue* rq, CSurface* surf, uint32_t count, uint32_t first) const {
	rq->addRenderOp(surf->mesh()->vertexBuffer(), surf->triangleBuffer(),	  //
					surf->material(), surf->mesh()->bindGroup(), m_bindGroup, //
					surf->triangleCount() * 3, count, 0, first, surf->mesh()->shadowsEnabled());
}

} // namespace sgd
