#include "spriterenderer.h"

#include "renderpipeline.h"
#include "sprite.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/spriterenderer.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	2,
	{
		bufferBindGroupLayoutEntry( //
			0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::ReadOnlyStorage),
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

void SpriteRenderer::onUpdate(CVec3r eye) {

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
	for (auto it = m_instances.begin(); it != m_instances.end(); ++instp, ++it) {
		instp->matrix = Mat4f((*it)->worldMatrix());
		instp->color = (*it)->color();
		instp->rect = (*it)->rect();
	}
	m_instanceBuffer->unlock();

	invalidate(true);
}

void SpriteRenderer::onValidate(GraphicsContext* gc) const {

	auto addOp = [&](Material* material, uint32_t count) {
		auto pipeline = getOrCreateRenderPipeline(gc, material, m_bindGroup, DrawMode::triangleList);
		auto& ops = m_renderOps[(int)renderPassType(material->blendMode())];
		auto first = ops.empty() ? 0 : ops.back().firstElement + ops.back().elementCount;
		ops.emplace_back(nullptr, nullptr, nullptr, material->bindGroup(), m_bindGroup, pipeline, count * 6, 1, first);
	};

	m_renderOps = {};

	Material* material = nullptr;
	uint32_t count = 0;

	for (Sprite* sprite : m_instances) {
		if (sprite->material() != material) {
			if (count) addOp(material, count);
			material = sprite->material();
			count = 0;
		}
		++count;
	}
	if (count) addOp(material, count);
}

} // namespace sgd
