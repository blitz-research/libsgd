#include "skinnedmeshrenderer.h"

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

static BindGroupDescriptor bindGroupDescriptor //
	(2,
	 {
		 bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::Uniform),
		 bufferBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::ReadOnlyStorage),
	 },
	 {vertexBufferLayout}, //
	 shaderSource);

RenderPass renderPass(BlendMode blendMode) {
	switch (blendMode) {
	case BlendMode::opaque:
		return RenderPass::opaque;
	case BlendMode::alpha:
	case BlendMode::additive:
	case BlendMode::multiply:
		return RenderPass::blend;
	}
	unreachable();
}

} // namespace

} // namespace sgd
namespace sgd {

SkinnedMeshRenderer::SkinnedMeshRenderer(CMesh* mesh) : m_mesh(mesh), m_bindGroup(new BindGroup(&bindGroupDescriptor)) {

	MeshUniforms meshUniforms;
	meshUniforms.tangentsEnabled = int(bool(m_mesh->flags() & MeshFlags::tangentsEnabled));

	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, &meshUniforms, sizeof(meshUniforms)));

	addDependency(m_mesh);
	addDependency(m_bindGroup);
}

SkinnedMeshInstance* SkinnedMeshRenderer::lockInstances(uint32_t count) {
//	log() << "### Locking instance buffer";
	m_instanceCount = count;
	if (m_instanceCount > m_instanceCapacity) {
		m_instanceCapacity = m_instanceCount;
		m_instanceBuffer = new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(SkinnedMeshInstance));
		log() << "### Reallocating instance buffer";
		m_bindGroup->setBuffer(1, m_instanceBuffer);
	}
	return (SkinnedMeshInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(SkinnedMeshInstance));
}

void SkinnedMeshRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}

void SkinnedMeshRenderer::onValidate(GraphicsContext* gc) const {
//	log() << "### Validating renderer";

	for (int i = 0; i < renderPassCount; ++i) m_renderOps[i].clear();
	m_renderPassMask = 0;

	m_vertexBuffer = m_mesh->vertexBuffer()->wgpuBuffer();
	m_indexBuffer = m_mesh->indexBuffer()->wgpuBuffer();

	for (int i = 0; i < m_mesh->surfaces().size(); ++i) {
		auto& surf = m_mesh->surfaces()[i];
		int rpass = (int)renderPass(surf.material->blendMode());

		auto pipeline = getOrCreateRenderPipeline(gc, surf.material, m_bindGroup, DrawMode::triangleList);

		m_renderOps[rpass].push_back({pipeline, surf.material->bindGroup()->wgpuBindGroup(), surf.firstTriangle * 3, surf.triangleCount * 3});

		m_renderPassMask |= 1 << rpass;
	}
}

void SkinnedMeshRenderer::onRender(GraphicsContext* gc) const {
	if (!m_instanceCount) return;

	auto& encoder = gc->wgpuRenderPassEncoder();

	encoder.SetVertexBuffer(0, m_vertexBuffer);
	encoder.SetIndexBuffer(m_indexBuffer, wgpu::IndexFormat::Uint32);
	encoder.SetBindGroup(2, m_bindGroup->wgpuBindGroup());

	for (auto& op : m_renderOps[(int)gc->renderPass()]) {
		encoder.SetBindGroup(1, op.bindGroup1);
		encoder.SetPipeline(op.pipeline);
		encoder.DrawIndexed(op.indexCount, m_instanceCount, op.firstIndex);
	}
}

} // namespace sgd
