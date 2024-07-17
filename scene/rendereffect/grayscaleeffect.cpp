#include "grayscaleeffect.h"

namespace sgd {

namespace {

auto shaderSource{
#include "grayscaleeffect.wgsl"
};

BindGroupDescriptor bindGroupDesc( //
	"grayscaleEffect",			   //
	BindGroupType::geometry,	   //
	{
		textureBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),
	},
	shaderSource, //
	{},			  //
	wgpu::PrimitiveTopology::TriangleStrip);

} // namespace

GrayscaleEffect::GrayscaleEffect() : m_bindGroup(new BindGroup(&bindGroupDesc)) {
}

void GrayscaleEffect::onValidate() {
	m_renderTarget = getOrCreateRenderTarget(sourceTexture()->size(), sourceTexture()->format());
	m_bindGroup->setTexture(0, sourceTexture());
}

void GrayscaleEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

	if (!m_pipeline) {
		m_pipeline = getOrCreateRenderPipeline(RenderPassType::effect,					 //
											   BlendMode::opaque,						 //
											   DepthFunc::undefined,					 //
											   CullMode::none,							 //
											   emptyBindGroup(BindGroupType::material),	 //
											   m_bindGroup,								 //
											   emptyBindGroup(BindGroupType::renderer)); //
	}

	auto pass = rc->beginRenderPass(RenderPassType::effect, m_renderTarget, nullptr, {}, 1, sceneBindings);

	pass.SetBindGroup(1, emptyBindGroup(BindGroupType::material)->wgpuBindGroup());
	pass.SetBindGroup(2, m_bindGroup->wgpuBindGroup());
	pass.SetBindGroup(3, emptyBindGroup(BindGroupType::renderer)->wgpuBindGroup());
	pass.SetPipeline(m_pipeline);
	pass.Draw(4);

	rc->endRenderPass();
}

} // namespace sgd
