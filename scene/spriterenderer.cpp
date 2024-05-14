#include "spriterenderer.h"

#include "sprite.h"

namespace sgd {

namespace {

auto shaderSource{
#include <graphics/shaders/spriterenderer.wgsl>
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

	auto inst = (SpriteInstance*)m_instanceBuffer->lock(0, m_instanceCount * sizeof(SpriteInstance));
	for (auto it = m_instances.begin(); it != m_instances.end(); ++it) {
		auto& worldMatrix =(*it)->worldMatrix();
		inst->matrix.i = {worldMatrix.r.i, 0};
		inst->matrix.j = {worldMatrix.r.j, 0};
		inst->matrix.k = {worldMatrix.r.k, 0};
		inst->matrix.t = {worldMatrix.t - eye, 1};
		inst->color = (*it)->color();
		inst->frame = (*it)->frame();
		++inst;
	}
	m_instanceBuffer->unlock();

	invalidate(true);
}

void SpriteRenderer::onValidate(GraphicsContext* gc) const {

	auto addOp = [&](Image* image, uint32_t count) {
		auto material=image->material();
		auto pipeline = getOrCreateRenderPipeline(gc, material, m_bindGroup, DrawMode::triangleList);
		auto& ops = m_renderOps[(int)renderPassType(material->blendMode())];
		auto first = ops.empty() ? 0 : ops.back().firstElement + ops.back().elementCount;
		ops.emplace_back(nullptr, nullptr, nullptr, material->bindGroup(), m_bindGroup, pipeline, count * 6, 1, first);
	};

	m_renderOps = {};

	Image* image{};
	uint32_t count{};

	for (Sprite* sprite : m_instances) {
		if (sprite->image() != image) {
			if (count) addOp(image, count);
			image=sprite->image();
			count = 1;
		}else {
			++count;
		}
	}
	if (count) addOp(image, count);
}

} // namespace sgd