#include "skyboxrenderer.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/skyboxrenderer.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	3,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // SkyboxUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
	},
	{}, shaderSource);

} // namespace

SkyboxRenderer::SkyboxRenderer() {

	m_bindGroup = new BindGroup(&sgd::bindGroupDescriptor);

	SkyboxUniforms uniforms{};
	m_uniformBuffer = new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms));
	m_bindGroup->setBuffer(0, m_uniformBuffer);

	skyTexture.changed.connect(this, [=](Texture* texture) { //
		m_bindGroup->setTexture(1, texture);
	});

	roughness.changed.connect(this, [=](float r) {
		float bias = r * 15.99f;
		m_uniformBuffer->update(&bias, offsetof(SkyboxUniforms, mipmapBias), sizeof(bias));
	});

	m_renderPassMask = 1 << (int)RenderPass::clear;
}

void SkyboxRenderer::onValidate(GraphicsContext* gc) const {
	m_pipeline = getOrCreateRenderPipeline(gc, nullptr, BlendMode::opaque, DepthFunc::undefined, CullMode::none, m_bindGroup,
										   DrawMode::triangleStrip);
}

void SkyboxRenderer::onRender(GraphicsContext* gc) const {
	auto& encoder = gc->wgpuRenderPassEncoder();
	encoder.SetBindGroup(1, emptyBindGroup(1)->wgpuBindGroup());
	encoder.SetBindGroup(2, m_bindGroup->wgpuBindGroup());
	encoder.SetPipeline(m_pipeline);
	encoder.Draw(4, 1);
}

} // namespace sgd
