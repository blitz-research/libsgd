#include "imagerenderer.h"

#include "../render/renderqueue.h"

namespace sgd {

namespace {

auto shaderSource{
#include "imagerenderer.wgsl"
};

BindGroupDescriptor bindGroupDesc( //
	"imageRenderer",			   //
	BindGroupType::renderer,	   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::ReadOnlyStorage),
	},
	shaderSource);

} // namespace

ImageRenderer::ImageRenderer()
	: m_bindGroup(new BindGroup(&bindGroupDesc)), //
	  m_instanceCapacity(1024),					  //
	  m_instanceBuffer(new Buffer(BufferType::storage, nullptr, m_instanceCapacity * sizeof(ImageInstance))) {

	m_bindGroup->setBuffer(0, m_instanceBuffer);
}

ImageInstance* ImageRenderer::lockInstances(uint32_t count) {
	if (count > m_instanceCapacity) {
		m_instanceCapacity = count;
		m_instanceBuffer->resize(m_instanceCapacity * sizeof(ImageInstance));
	}
	return (ImageInstance*)m_instanceBuffer->lock(0, count * sizeof(ImageInstance));
}

void ImageRenderer::unlockInstances() {
	m_instanceBuffer->unlock();
}

void ImageRenderer::render(RenderQueue* rq, CImage* image, uint32_t first, uint32_t count) {
	rq->addRenderOp(nullptr, nullptr,									//
					image->material(), image->bindGroup(), m_bindGroup, //
					count * 6, 1, first * 6, false);					//
}

} // namespace sgd
