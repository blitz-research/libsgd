#include "spriterenderer.h"

#include "renderpipeline.h"
#include "sprite.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/spriterenderer.wgsl"
};

BindGroupDescriptor bindGroupDescriptor //
	(2,
	 {
		 bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::ReadOnlyStorage),
	 },
	 {}, //
	 shaderSource);
} // namespace

SpriteRenderer::SpriteRenderer() : m_bindGroup(new BindGroup(&bindGroupDescriptor)) {

	addDependency(m_bindGroup);
}

void SpriteRenderer::add(Sprite* sprite) {
	m_instances.emplace_back(sprite);
}

void SpriteRenderer::remove(Sprite* sprite) {
	sgd::remove(m_instances, sprite);
}

void SpriteRenderer::onUpdate(CVec3f eye) {

	// Sort sprites
	auto cmp = [=](const Sprite* lhs, const Sprite* rhs) {
		return length(rhs->worldMatrix().t - eye) < length(lhs->worldMatrix().t - eye);
	};
	std::sort(m_instances.begin(), m_instances.end(), cmp);

	m_instanceCount = m_instances.size();
	if (m_instanceCount > m_instanceCapacity) {
		m_instanceCapacity = m_instanceCount;
		m_instanceBuffer = new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(SpriteInstance));
		m_bindGroup->setBuffer(0, m_instanceBuffer);
	}

	auto instp = (SpriteInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(SpriteInstance));

	m_renderOps[0].clear();
	m_renderOps[1].clear();
	Material* material = nullptr;
	uint32_t firstInstance = 0;
	uint32_t instanceCount = 0;
	for (auto it = m_instances.begin(); it != m_instances.end(); ++instanceCount, ++instp, ++it) {
		if ((*it)->material() != material) {
			material = (*it)->material();
			if (instanceCount) {
				m_renderOps[material->blendMode() == BlendMode::opaque].push_back(RenderOp{firstInstance, instanceCount});
				firstInstance += instanceCount;
				instanceCount = 0;
			}
		}
		instp->matrix = Mat4f((*it)->worldMatrix());
		instp->color = (*it)->color();
		instp->rect = (*it)->rect();
	}
	if (instanceCount) {
		m_renderOps[material->blendMode() == BlendMode::opaque].push_back(RenderOp{firstInstance, instanceCount});
	}

	m_instanceBuffer->unlock();

	invalidate(true);
}

void SpriteRenderer::onValidate(GraphicsContext* gc) const {
	for (auto& ops : m_renderOps) {
		for (auto& op : ops) {
			auto material = m_instances[op.firstInstance]->material();
			material->validate(gc);
			op.material = material->bindGroup()->wgpuBindGroup();
			op.pipeline = getOrCreateRenderPipeline(gc, material, m_bindGroup, DrawMode::triangleList);
		}
	}
	m_renderPassMask = 0;
	if (!m_renderOps[1].empty()) m_renderPassMask |= 1 << (int)RenderPass::opaque;
	if (!m_renderOps[0].empty()) m_renderPassMask |= 1 << (int)RenderPass::blend;
}

void SpriteRenderer::onRender(GraphicsContext* gc) const {
	auto& ops = gc->renderPass() == RenderPass::opaque ? m_renderOps[1] : m_renderOps[0];
	auto& rpass = gc->wgpuRenderPassEncoder();
	rpass.SetBindGroup(2, m_bindGroup->wgpuBindGroup());
	for (auto& op : ops) {
		rpass.SetBindGroup(1, op.material);
		rpass.SetPipeline(op.pipeline);
		rpass.Draw(op.instanceCount * 6, 1, op.firstInstance * 6);
	}
}

} // namespace sgd
