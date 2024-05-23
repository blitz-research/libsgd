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
	{vertexBufferLayout}, //
	shaderSource);

} // namespace

} // namespace sgd
namespace sgd {

SkinnedMeshRenderer::SkinnedMeshRenderer(CMesh* mesh)
	: m_mesh(mesh),										//
	  m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_instanceCapacity(8),							//
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
		m_instanceBuffer->resize(m_instanceCapacity * sizeof(SkinnedMeshInstance));
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

		for (Surface* surf : m_mesh->surfaces()) {

			int rpass = (int)renderPassType(surf->material()->blendMode());

			auto pipeline = getOrCreateRenderPipeline(gc, surf->material(), m_bindGroup, DrawMode::triangleList);

			m_renderOps[rpass].emplace_back(   //
				m_mesh->vertexBuffer(),		   //
				nullptr,					   //
				surf->triangleBuffer(),		   //
				surf->material()->bindGroup(), //
				m_bindGroup,				   //
				pipeline,					   //
				surf->triangleCount() * 3, m_instanceCount, 0);

			if (surf->material()->blendMode() == BlendMode::opaque) {

				auto shadowPipeline = getOrCreateShadowPipeline(gc, m_bindGroup, DrawMode::triangleList);

				m_renderOps[(int)RenderPassType::shadow].emplace_back( //
					m_mesh->vertexBuffer(),							   //
					nullptr,										   //
					surf->triangleBuffer(),							   //
					shadowBindGroup(),								   //
					m_bindGroup,									   //
					shadowPipeline,									   //
					surf->triangleCount() * 3, m_instanceCount, 0);
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
