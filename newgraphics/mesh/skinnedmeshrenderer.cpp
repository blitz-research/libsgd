#include "skinnedmeshrenderer.h"

#include "../render/rendercontext.h"

namespace sgd {

namespace {

struct alignas(16) SkinnedMeshUniforms {
	int32_t tangentsEnabled{0};
};

auto shaderSource{
#include "skinnedmeshrenderer.wgsl"
};

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x3, 40, 3}, // Vec3f texCoords0
	{wgpu::VertexFormat::Float32x4, 52, 4}, // Vec4f color
	{wgpu::VertexFormat::Uint8x4, 68, 5},	// uint8_t joints[4]
	{wgpu::VertexFormat::Float32x4, 72, 6}, // float weights[4];
};
static_assert(sizeof(Vertex) == 88);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexBufferAttribs),
												  vertexBufferAttribs};

BindGroupDescriptor bindGroupDescriptor( //
	"skinnedMeshRenderer",				 //
	BindGroupType::renderer,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
		bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::ReadOnlyStorage),
	},
	shaderSource,		  //
	{vertexBufferLayout}, //
	wgpu::PrimitiveTopology::TriangleList);

} // namespace

} // namespace sgd
namespace sgd {

SkinnedMeshRenderer::SkinnedMeshRenderer(CMesh* mesh)
	: m_mesh(mesh),										//
	  m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_instanceCapacity(8),							//
	  m_instanceBuffer(new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(SkinnedMeshInstance))) {

	SkinnedMeshUniforms meshUniforms;
	meshUniforms.tangentsEnabled = int(bool(m_mesh->flags() & MeshFlags::tangentsEnabled));
	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, &meshUniforms, sizeof(meshUniforms)));
	m_bindGroup->setBuffer(1, m_instanceBuffer);
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
void SkinnedMeshRenderer::render(RenderContext* rc) const {

	rc->setVertexBuffer(m_mesh->vertexBuffer());
	rc->setGeometry(emptyBindGroup(BindGroupType::geometry));
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
		rc->setIndexBuffer(surf->triangleBuffer());
		rc->setMaterial(material);
		rc->render(surf->triangleCount() * 3, m_instanceCount, 0);
	}
}

} // namespace sgd
