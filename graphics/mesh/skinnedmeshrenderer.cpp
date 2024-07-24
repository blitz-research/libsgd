#include "skinnedmeshrenderer.h"

#include "../render/renderqueue.h"

namespace sgd {

namespace {

auto shaderSource{
#include "skinnedmeshrenderer.wgsl"
};

wgpu::VertexAttribute vertexAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x3, 40, 3}, // Vec3f texCoords0
	{wgpu::VertexFormat::Float32x4, 52, 4}, // Vec4f color
	{wgpu::VertexFormat::Uint8x4, 68, 5},	// uint8_t joints[4]
	{wgpu::VertexFormat::Float32x4, 72, 6}, // float weights[4];
};
static_assert(sizeof(Vertex) == 88);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexAttribs),
												  vertexAttribs};

BindGroupDescriptor bindGroupDescriptor( //
	"skinnedMeshRenderer",				 //
	BindGroupType::renderer,
	{
		// renderer_instances: SkinnedMeshInstance[]
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::ReadOnlyStorage),
	},
	shaderSource,							//
	{vertexBufferLayout},					//
	wgpu::PrimitiveTopology::TriangleList); //

} // namespace

SkinnedMeshRenderer::SkinnedMeshRenderer(CMesh* mesh)
	: m_mesh(mesh),										//
	  m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_instanceCapacity(8),							//
	  m_instanceBuffer(new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(SkinnedMeshInstance))) {

	m_bindGroup->setBuffer(0, m_instanceBuffer);
}

SkinnedMeshInstance* SkinnedMeshRenderer::lockInstances(uint32_t count) {
	if (count > m_instanceCapacity) {
		m_instanceCapacity = count;
		m_instanceBuffer->resize(m_instanceCapacity * sizeof(SkinnedMeshInstance));
	}
	if (count != m_instanceCount) {
		m_instanceCount = count;
	}
	return (SkinnedMeshInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(SkinnedMeshInstance));
}

void SkinnedMeshRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}
void SkinnedMeshRenderer::render(RenderQueue* rq) const {
	for (Surface* surf : m_mesh->surfaces()) {
		rq->addRenderOp(m_mesh->vertexBuffer(), surf->triangleBuffer(),		//
						surf->material(), m_mesh->bindGroup(), m_bindGroup, //
						surf->triangleCount() * 3, m_instanceCount, 0, m_mesh->shadowsEnabled());
	}
}

} // namespace sgd
