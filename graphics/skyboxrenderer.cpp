#include "skyboxrenderer.h"

#include "shaders/uniforms.h"

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
							bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment,
													   wgpu::BufferBindingType::Uniform), // SkyboxUniforms
							textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
							samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
						},
						{}, //
						shaderSource);

} // namespace

SkyboxRenderer::SkyboxRenderer() {
	m_renderPassMask = 1 << (int)RenderPass::clear;

	m_bindGroup2 = new BindGroup(&sgd::bindGroupDescriptor);

	SkyboxUniforms uniforms{};
	m_bindGroup2->setBuffer(0, new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms)));

	skyTexture.changed.connect(this, [=](Texture* texture) { //
		m_bindGroup2->setTexture(1, texture);
	});

	roughness.changed.connect(this, [=](float roughness) {
		float bias = roughness * 15.99f;
		m_bindGroup2->getBuffer(0)->update(&bias,offsetof(SkyboxUniforms, mipmapBias), sizeof(bias));
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
