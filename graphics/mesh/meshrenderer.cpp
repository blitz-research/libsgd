#include "meshrenderer.h"

#include "../render/renderqueue.h"

namespace sgd {

namespace {

BindGroupDescriptor meshRendererDesc( //
	"meshRenderer",					  //
	BindGroupType::renderer,		  //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::ReadOnlyStorage),
	});

} // namespace

MeshRenderer::MeshRenderer(CMesh* mesh)				 //
	: m_mesh(mesh),									 //
	  m_bindGroup(new BindGroup(&meshRendererDesc)), //
	  m_instanceCapacity(8),						 //
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

void MeshRenderer::render(RenderQueue* rq) const {
	for (Surface* surf : m_mesh->surfaces()) {
		rq->addRenderOp(m_mesh->vertexBuffer(), surf->triangleBuffer(),		//
						surf->material(), m_mesh->bindGroup(), m_bindGroup, //
						surf->triangleCount() * 3, m_instanceCount, 0, m_mesh->shadowsEnabled());
	}
}

} // namespace sgd
