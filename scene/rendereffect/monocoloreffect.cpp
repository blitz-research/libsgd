#include "monocoloreffect.h"

namespace sgd {

namespace {

struct alignas(16) MonocolorEffectUniforms {
	Vec4f color;
};

auto shaderSource{
#include "monocoloreffect.wgsl"
};

BindGroupDescriptor bindGroupDesc( //
	"monocolorEffect",			   //
	BindGroupType::geometry,	   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
	},
	shaderSource, //
	{},			  //
	wgpu::PrimitiveTopology::TriangleStrip);

} // namespace

MonocolorEffect::MonocolorEffect() : m_bindGroup(new BindGroup(&bindGroupDesc)) {
	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, nullptr, sizeof(MonocolorEffectUniforms)));
	m_bindGroup->setTexture(1, blackTexture());
}

Texture* MonocolorEffect::onValidate(Texture* sourceTexture, Texture* depthBuffer) {

	m_renderTarget = getOrCreateRenderTarget(sourceTexture->size(), sourceTexture->format());

	MonocolorEffectUniforms uniforms{color()};
	((Buffer*)m_bindGroup->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));

	m_bindGroup->setTexture(1, sourceTexture);

	return m_renderTarget;
}

void MonocolorEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

	if (!m_pipeline) {
		m_pipeline = getOrCreateRenderPipeline(		  //
			RenderPassType::opaque,					  //
			BlendMode::opaque,						  //
			DepthFunc::undefined,					  //
			CullMode::none,							  //
			emptyBindGroup(BindGroupType::material),  //
			m_bindGroup,							  //
			emptyBindGroup(BindGroupType::renderer)); //
	}

	auto pass = rc->beginRenderPass(RenderPassType::opaque, m_renderTarget, nullptr, {}, 1, sceneBindings);

	pass.SetBindGroup(1, emptyBindGroup(BindGroupType::material)->wgpuBindGroup());
	pass.SetBindGroup(2, m_bindGroup->wgpuBindGroup());
	pass.SetBindGroup(3, emptyBindGroup(BindGroupType::renderer)->wgpuBindGroup());
	pass.SetPipeline(m_pipeline);
	pass.Draw(4);

	rc->endRenderPass();
}

} // namespace sgd
