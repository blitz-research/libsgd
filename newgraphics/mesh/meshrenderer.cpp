#include "meshrenderer.h"

#include "../render/rendercontext.h"

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

void MeshRenderer::render(RenderContext* rc) const {

	rc->setVertexBuffer(m_mesh->vertexBuffer());
	rc->setGeometry(m_mesh->bindGroup());
	rc->setRenderer(m_bindGroup);

	for (Surface* surf : m_mesh->surfaces()) {
		auto material = surf->material();
		switch (rc->renderPassType()) {
		case RenderPassType::shadow:
		case RenderPassType::opaque:
			if (material->blendMode() != BlendMode::opaque && material->blendMode() != BlendMode::alphaMask) continue;
			break;
		case RenderPassType::blend:
			if (material->blendMode() != BlendMode::alphaMask) continue;
			break;
		case RenderPassType::effect:
			continue;
		}
		rc->setMaterial(material);
		rc->setIndexBuffer(surf->triangleBuffer());
		rc->render(surf->triangleCount() * 3, m_instanceCount, 0);
	}
}

} // namespace sgd
