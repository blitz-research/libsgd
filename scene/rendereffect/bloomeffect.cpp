#include "bloomeffect.h"

namespace sgd {

namespace {

auto shaderSource{
#include "bloomeffect.wgsl"
};

BindGroupDescriptor bindGroupDesc( //
	"bloomEffect",				   //
	BindGroupType::geometry,	   //
	{
		textureBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),
	},
	shaderSource, //
	{},			  //
	wgpu::PrimitiveTopology::TriangleStrip);

} // namespace

BloomEffect::BloomEffect() {
	for (int i = 0; i < 3; ++i) {
		m_bindGroups[i] = new BindGroup(&bindGroupDesc);
	}
}

Texture* BloomEffect::onValidate(Texture* sourceTexture) {

	m_renderTargets[0] = getOrCreateRenderTarget(sourceTexture->size() / 2u, sourceTexture->format());
	m_renderTargets[1] = getOrCreateRenderTarget(sourceTexture->size() / 4u, sourceTexture->format());
	m_renderTargets[2] = getOrCreateRenderTarget(sourceTexture->size() / 8u, sourceTexture->format());

	m_bindGroups[0]->setTexture(0, sourceTexture);
	m_bindGroups[1]->setTexture(0, m_renderTargets[0]);
	m_bindGroups[2]->setTexture(0, m_renderTargets[1]);

	m_pipeline = {};

	return m_renderTargets[1];
}

void BloomEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

	if (!m_pipeline) {
		m_pipeline = getOrCreateRenderPipeline(RenderPassType::effect,					 //
											   BlendMode::opaque,						 //
											   DepthFunc::undefined,					 //
											   CullMode::none,							 //
											   emptyBindGroup(BindGroupType::material),	 //
											   m_bindGroups[0],							 //
											   emptyBindGroup(BindGroupType::renderer)); //
	}

	for (int i = 0; i < 2; ++i) {
		// SGD_LOG << "Being render pass"<<i;
		auto pass = rc->beginRenderPass(RenderPassType::effect, m_renderTargets[i], nullptr, {}, 1, sceneBindings);

		pass.SetBindGroup(1, emptyBindGroup(BindGroupType::material)->wgpuBindGroup());
		pass.SetBindGroup(2, m_bindGroups[i]->wgpuBindGroup());
		pass.SetBindGroup(3, emptyBindGroup(BindGroupType::renderer)->wgpuBindGroup());
		pass.SetPipeline(m_pipeline);
		pass.Draw(4);

		rc->endRenderPass();
	}
}

} // namespace sgd
