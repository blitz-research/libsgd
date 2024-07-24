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

Array<BindGroupDescriptor*, BloomEffect::numPasses> bindGroupDescriptors;

} // namespace

BloomEffect::BloomEffect() {

	if (!bindGroupDescriptors[0]) {
		for (int pass = 0; pass < numPasses; ++pass) {
			String header = "#define BLOOM_PASS_" + toString(pass) + " 1\n";
			auto desc = new BindGroupDescriptor( //
				"bloomEffect", BindGroupType::geometry,
				{
					textureBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
					samplerBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),
				},
				header + shaderSource, //
				{},					   //
				wgpu::PrimitiveTopology::TriangleStrip);
			bindGroupDescriptors[pass] = desc;
		}
	}

	for (int pass = 0; pass < numPasses; ++pass) {
		m_passes[pass].bindGroup = new BindGroup(bindGroupDescriptors[pass]);
	}
}

Texture* BloomEffect::onValidate(Texture* sourceTexture, Texture* depthBuffer) {

	m_passes[0].renderTarget = getOrCreateRenderTarget(sourceTexture->size() / 2u, sourceTexture->format());
	m_passes[1].renderTarget = getOrCreateRenderTarget(sourceTexture->size() / 4u, sourceTexture->format());
	m_passes[2].renderTarget = getOrCreateRenderTarget(sourceTexture->size() / 8u, sourceTexture->format());
	m_passes[3].renderTarget = getOrCreateRenderTarget(sourceTexture->size() / 8u, sourceTexture->format());
	m_passes[4].renderTarget = sourceTexture;

	m_passes[0].bindGroup->setTexture(0, sourceTexture);
	for (int pass = 1; pass < numPasses; ++pass) {
		m_passes[pass].bindGroup->setTexture(0, m_passes[pass - 1].renderTarget);
	}

	for (auto& pass : m_passes) pass.pipeline = {};

	return m_passes[numPasses - 1].renderTarget;
}

void BloomEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

	if (!m_passes[0].pipeline) {
		for (int pass = 0; pass < numPasses; ++pass) {
			auto blendMode = pass < numPasses - 1 ? BlendMode::opaque : BlendMode::alphaBlend;
			auto rpassType = blendMode == BlendMode::opaque ? RenderPassType::opaque : RenderPassType::blend;
			auto pipeline = getOrCreateRenderPipeline(rpassType,					//
													  blendMode,								//
													  DepthFunc::undefined,						//
													  CullMode::none,							//
													  emptyBindGroup(BindGroupType::material),	//
													  m_passes[pass].bindGroup,					//
													  emptyBindGroup(BindGroupType::renderer)); //
			m_passes[pass].renderPassType = rpassType;
			m_passes[pass].pipeline = pipeline;
		}
	}

	for (auto& pass : m_passes) {

		auto rpass = rc->beginRenderPass(pass.renderPassType, pass.renderTarget, nullptr, {}, 1, sceneBindings);

		rpass.SetBindGroup(1, emptyBindGroup(BindGroupType::material)->wgpuBindGroup());
		rpass.SetBindGroup(2, pass.bindGroup->wgpuBindGroup());
		rpass.SetBindGroup(3, emptyBindGroup(BindGroupType::renderer)->wgpuBindGroup());
		rpass.SetPipeline(pass.pipeline);
		rpass.Draw(4);

		rc->endRenderPass();
	}
}

} // namespace sgd
