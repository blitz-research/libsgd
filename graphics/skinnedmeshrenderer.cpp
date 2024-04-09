#include "skinnedmeshrenderer.h"

#include "shadowmaterial.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/skinnedmeshrenderer.wgsl"
};

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x2, 40, 3}, // Vec2f texCoords0
	{wgpu::VertexFormat::Float32x4, 48, 4}, // Vec4f color
	{wgpu::VertexFormat::Uint8x4, 64, 5},	// uint8_t joints[4]
	{wgpu::VertexFormat::Float32x4, 68, 6}, // float weights[4];
};
static_assert(sizeof(Vertex) == 84);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexBufferAttribs),
												  vertexBufferAttribs};

BindGroupDescriptor bindGroupDescriptor //
	(2,
	 {
		 bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::Uniform),
		 bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::ReadOnlyStorage),
	 },
	 {vertexBufferLayout}, //
	 shaderSource);

} // namespace

} // namespace sgd
namespace sgd {

SkinnedMeshRenderer::SkinnedMeshRenderer(CMesh* mesh)
	: m_mesh(mesh), //
	  m_bindGroup(new BindGroup(&bindGroupDescriptor)),
	  m_instanceBuffer(new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(SkinnedMeshInstance))) {

	MeshUniforms meshUniforms;
	meshUniforms.tangentsEnabled = int(bool(m_mesh->flags() & MeshFlags::tangentsEnabled));
	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, &meshUniforms, sizeof(meshUniforms)));
	m_bindGroup->setBuffer(1, m_instanceBuffer);

	addDependency(m_mesh, [=] {
		m_rebuildRenderOps = true;
		invalidate();
	});

	addDependency(m_bindGroup, [=] {
		m_rebuildRenderOps = true;
		invalidate();
	});

	addDependency(m_instanceBuffer, [=] {
		m_rebuildRenderOps = true;
		invalidate();
	});
}

SkinnedMeshInstance* SkinnedMeshRenderer::lockInstances(uint32_t count) {
	if (count > m_instanceCapacity) {
		m_instanceCapacity = count;
		m_instanceBuffer = new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(SkinnedMeshInstance));
	}
	if (count != m_instanceCount) {
		m_instanceCount = count;
		m_updateInstanceCounts = true;
		invalidate();
	}
	return (SkinnedMeshInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(SkinnedMeshInstance));
}

void SkinnedMeshRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}

void SkinnedMeshRenderer::onValidate(GraphicsContext* gc) const {

	if (m_rebuildRenderOps) {

		m_renderOps = {};

		for (int i = 0; i < m_mesh->surfaces().size(); ++i) {

			auto& surf = m_mesh->surfaces()[i];
			int rpass = (int)renderPassType(surf.material->blendMode());

			auto pipeline = getOrCreateRenderPipeline(gc, surf.material, m_bindGroup, DrawMode::triangleList);

			m_renderOps[rpass].emplace_back( //
				m_mesh->vertexBuffer(),		 //
				nullptr,					 //
				m_mesh->indexBuffer(),		 //
				surf.material->bindGroup(),	 //
				m_bindGroup,				 //
				pipeline,					 //
				surf.triangleCount * 3, m_instanceCount, surf.firstTriangle * 3);

			if (surf.material->blendMode() == BlendMode::opaque) {

				auto shadowPipeline = getOrCreateShadowPipeline(gc, m_bindGroup, DrawMode::triangleList);

				m_renderOps[(int)RenderPassType::shadow].emplace_back( //
					m_mesh->vertexBuffer(),							   //
					nullptr,										   //
					m_mesh->indexBuffer(),							   //
					shadowBindGroup(),								   //
					m_bindGroup,									   //
					shadowPipeline,									   //
					surf.triangleCount * 3, m_instanceCount, surf.firstTriangle * 3);
			}
		}

		m_rebuildRenderOps = m_updateInstanceCounts = false;
	}

	if (m_updateInstanceCounts) {
		for (auto& ops : m_renderOps) {
			for (auto& op : ops) {
				op.instanceCount = m_instanceCount;
			}
		}
		m_updateInstanceCounts = false;
	}
}

} // namespace sgd
