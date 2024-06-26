#include "meshrenderer.h"

#include "shaders/uniforms.h"
#include "shadowmaterial.h"

namespace sgd {

namespace {

auto shaderSource =
#include "shaders/meshrenderer.wgsl"
	;

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x3, 40, 3}, // Vec3f texCoords
	{wgpu::VertexFormat::Float32x4, 52, 4}, // Vec4f color
											// uchar8_t joints[4];
											// float weights[4];
};
static_assert(sizeof(Vertex) == 88);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexBufferAttribs),
												  vertexBufferAttribs};
#if 0
inline wgpu::VertexAttribute instanceBufferAttribs[]{
	{wgpu::VertexFormat::Float32x4, 0, 8},	 //
	{wgpu::VertexFormat::Float32x4, 16, 9},	 //
	{wgpu::VertexFormat::Float32x4, 32, 10}, //
	{wgpu::VertexFormat::Float32x4, 48, 11}, //
	{wgpu::VertexFormat::Float32x4, 64, 12}, //
};
static_assert(sizeof(MeshInstance) == 80);

wgpu::VertexBufferLayout const instanceBufferLayout{sizeof(MeshInstance), wgpu::VertexStepMode::Instance,
													std::size(instanceBufferAttribs), instanceBufferAttribs};
#endif

BindGroupDescriptor bindGroupDescriptor( //
	"meshRenderer",						 //
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

MeshRenderer::MeshRenderer(CMesh* mesh)					//
	: m_mesh(mesh),										//
	  m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_instanceCapacity(8),							//
	  m_instanceBuffer(new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(MeshInstance))) {

	MeshUniforms meshUniforms;
	meshUniforms.tangentsEnabled = int(bool(m_mesh->flags() & MeshFlags::tangentsEnabled));
	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, &meshUniforms, sizeof(meshUniforms)));
	m_bindGroup->setBuffer(1, m_instanceBuffer);

	m_mesh->shadowsEnabled.changed.connect(this, [=](bool) {
		m_rebuildRenderOps = true;
		invalidate();
	});

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

MeshInstance* MeshRenderer::lockInstances(uint32_t count) {
	if (count > m_instanceCapacity) {
		m_instanceCapacity = count;
		m_instanceBuffer->resize(m_instanceCapacity * sizeof(MeshInstance));
	}
	if (count != m_instanceCount) {
		m_instanceCount = count;
		m_updateInstanceCounts = true;
		invalidate();
	}
	return (MeshInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(MeshInstance));
}

void MeshRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}

void MeshRenderer::onValidate(GraphicsContext* gc) const {

	if (m_rebuildRenderOps) {

		m_renderOps = {};

		for (Surface* surf : m_mesh->surfaces()) {
			addRenderOp(gc,						   //
						surf->material(),		   //
						nullptr,//
						m_mesh->vertexBuffer(),	   //
						surf->triangleBuffer(),	   //
						m_bindGroup,			   //
						DrawMode::triangleList,	   //
						surf->triangleCount() * 3, //
						m_instanceCount,		   //
						0,						   //
						m_mesh->shadowsEnabled());
		}
		m_rebuildRenderOps = m_updateInstanceCounts = false;
		return;
	}

	if (m_updateInstanceCounts) {
		m_updateInstanceCounts = false;
		for (auto& ops : m_renderOps) {
			for (auto& op : ops) {
				op.instanceCount = m_instanceCount;
			}
		}
	}
}

} // namespace sgd
