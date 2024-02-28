#include "skyboxrenderer.h"

namespace sgd {

namespace {

static const BindGroupDescriptor emptyDescs[]{				   //
											  {0, {}, {}, {}}, //
											  {1, {}, {}, {}}, //
											  {2, {}, {}, {}}};

auto shaderSource =
#include "shaders/skyboxrenderer.wgsl"
	;

BindGroupDescriptor
	bindGroupDescriptor(3, //
						{
							textureBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
							samplerBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),
						},
						{}, //
						shaderSource);

} // namespace

SkyboxRenderer::SkyboxRenderer() {
	m_renderPassMask = 1 << (int)RenderPass::clear;

	m_bindGroup2 = new BindGroup(&sgd::bindGroupDescriptor);

	skyTexture.changed.connect(this, [=](Texture* texture) { //
		m_bindGroup2->setTexture(0, texture);
	});
}

void SkyboxRenderer::onValidate(GraphicsContext* gc) const {

	m_pipeline = getRenderPipeline(gc, emptyBindGroup(1), m_bindGroup2, BlendMode::opaque, DepthFunc::undefined, CullMode::none,
								   DrawMode::triangleStrip);
}

void SkyboxRenderer::onRender(GraphicsContext* gc) const {
	auto& encoder = gc->wgpuRenderPassEncoder();
	encoder.SetBindGroup(1, emptyBindGroup(1)->wgpuBindGroup());
	encoder.SetBindGroup(2, m_bindGroup2->wgpuBindGroup());
	encoder.SetPipeline(m_pipeline);
	encoder.Draw(4, 1);
}

} // namespace sgd
