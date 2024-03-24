#include "meshrenderer.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/meshrenderer.wgsl"
};

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x2, 40, 3}, // Vec2f texCoords0
	{wgpu::VertexFormat::Float32x4, 48, 4}, // Vec4f color
	//	{wgpu::VertexFormat::Uint32, 64, 5},	// uchar8_t joints[4];
	//{wgpu::VertexFormat::Float32x4, 68, 6}, // float weights[4];
};
static_assert(sizeof(Vertex) == 84);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexBufferAttribs),
												  vertexBufferAttribs};

inline static wgpu::VertexAttribute instanceBufferAttribs[]{
	{wgpu::VertexFormat::Float32x4, 0, 8},	 //
	{wgpu::VertexFormat::Float32x4, 16, 9},	 //
	{wgpu::VertexFormat::Float32x4, 32, 10}, //
	{wgpu::VertexFormat::Float32x4, 48, 11}, //
	{wgpu::VertexFormat::Float32x4, 64, 12}, //
};
static_assert(sizeof(MeshInstance) == 80);

wgpu::VertexBufferLayout const instanceBufferLayout{sizeof(MeshInstance), wgpu::VertexStepMode::Instance,
													std::size(instanceBufferAttribs), instanceBufferAttribs};

static BindGroupDescriptor bindGroupDescriptor //
	(2,
	 {
		 bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::Uniform),
	 },
	 {vertexBufferLayout, instanceBufferLayout}, //
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

MeshRenderer::MeshRenderer(CMesh* mesh) : m_mesh(mesh), m_bindGroup(new BindGroup(&bindGroupDescriptor)) {

	MeshUniforms meshUniforms;
	meshUniforms.tangentsEnabled = int(bool(m_mesh->flags() & MeshFlags::tangentsEnabled));
	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, &meshUniforms, sizeof(meshUniforms)));

	addDependency(m_mesh);
	addDependency(m_bindGroup);
}

MeshInstance* MeshRenderer::lockInstances(uint32_t count) {
	m_instanceCount = count;
	if (m_instanceCount > m_instanceCapacity) {
		m_instanceCapacity = m_instanceCount;
		m_instanceBuffer = new Buffer(BufferType::instance, nullptr, m_instanceCapacity * sizeof(MeshInstance));
	}
	return (MeshInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(MeshInstance));
}

void MeshRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}

void MeshRenderer::onValidate(GraphicsContext* gc) const {

	for (int i = 0; i < renderPassCount; ++i) m_renderOps[i].clear();
	m_renderPassMask = 0;

	for (int i = 0; i < m_mesh->surfaces().size(); ++i) {
		auto& surf = m_mesh->surfaces()[i];
		int rpass = (int)renderPass(surf.material->blendMode());

		auto pipeline = getOrCreateRenderPipeline(gc, surf.material, m_bindGroup, DrawMode::triangleList);

		m_renderOps[rpass].push_back({pipeline, surf.material->bindGroup(), surf.firstTriangle * 3, surf.triangleCount * 3});

		m_renderPassMask |= 1 << rpass;
	}
}

void MeshRenderer::onRender(GraphicsContext* gc) const {
	if (!m_instanceCount) return;

	auto& encoder = gc->wgpuRenderPassEncoder();

	encoder.SetVertexBuffer(0, m_mesh->vertexBuffer()->wgpuBuffer());
	encoder.SetVertexBuffer(1, m_instanceBuffer->wgpuBuffer());
	encoder.SetIndexBuffer(m_mesh->indexBuffer()->wgpuBuffer(), wgpu::IndexFormat::Uint32);
	encoder.SetBindGroup(2, m_bindGroup->wgpuBindGroup());

	for (auto& op : m_renderOps[(int)gc->renderPass()]) {

		encoder.SetBindGroup(1, op.material->wgpuBindGroup());
		encoder.SetPipeline(op.pipeline);

		encoder.DrawIndexed(op.indexCount, m_instanceCount, op.firstIndex);
	}
}

} // namespace sgd
