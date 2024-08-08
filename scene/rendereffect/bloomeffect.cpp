#include "bloomeffect.h"

#include "gaussianfilter.h"

namespace sgd {

namespace {

struct alignas(16) BloomEffectUniforms {
	Vec2f texCoordScale;
	uint32_t kernelSize{};
	uint32_t padding{};
	Vec4f kernel[64];
};

auto shaderSource{
#include "bloomeffect.wgsl"
};

Array<BindGroupDescriptor*, BloomEffect::numPasses> bindGroupDescriptors;

} // namespace

BloomEffect::BloomEffect() {

	if (!bindGroupDescriptors[0]) {
		for (int pass = 0; pass < numPasses; ++pass) {
			String header = "#define BLOOM_PASS_" + toString(pass) + " 1\n";
			auto desc = new BindGroupDescriptor( //
				"bloomEffect", BindGroupType::geometry,
				{
					bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
					textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
					samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
				},
				header + shaderSource, //
				{},					   //
				wgpu::PrimitiveTopology::TriangleStrip);
			bindGroupDescriptors[pass] = desc;
		}
	}

	for (int pass = 0; pass < numPasses; ++pass) {
		m_passes[pass].bindGroup = new BindGroup(bindGroupDescriptors[pass]);
		m_passes[pass].bindGroup->setBuffer(0, new Buffer(BufferType::uniform, nullptr, sizeof(BloomEffectUniforms)));
	}
}

Texture* BloomEffect::onValidate(Texture* sourceTexture, Texture* depthBuffer) {

	// Update uniforms
	{
		auto r = std::min(std::max(this->radius(), 1u), 31u);
		auto kernelSize = r * 2 + 1;
		// https://dsp.stackexchange.com/a/74157
		float sigma = (float)r / 2;
		auto kernel = generateKernel(kernelSize, sigma);

		BloomEffectUniforms uniforms{};
		uniforms.kernelSize = kernelSize;
		std::memcpy(&uniforms.kernel, kernel.data(), sizeof(Vec4f) * kernelSize);

		uniforms.texCoordScale = Vec2f(1.0f / (float)sourceTexture->size().x, 0.0f);
		((Buffer*)m_passes[0].bindGroup->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));
		((Buffer*)m_passes[1].bindGroup->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));

		uniforms.texCoordScale = Vec2f(0.0f, 1.0f / (float)sourceTexture->size().y);
		((Buffer*)m_passes[2].bindGroup->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));
	}

	// Update render targets
	{

		m_passes[0].renderTarget = getOrCreateRenderTarget(sourceTexture->size(), sourceTexture->format());
		m_passes[1].renderTarget = getOrCreateRenderTarget(sourceTexture->size(), sourceTexture->format());
		m_passes[2].renderTarget = sourceTexture;

		m_passes[0].bindGroup->setTexture(1, sourceTexture);
		for (int pass = 1; pass < numPasses; ++pass) {
			m_passes[pass].bindGroup->setTexture(1, m_passes[pass - 1].renderTarget);
		}

		for (auto& pass : m_passes) pass.pipeline = {};
	}

	return sourceTexture;
}

void BloomEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

	if (!m_passes[0].pipeline) {
		for (int pass = 0; pass < numPasses; ++pass) {
			auto blendMode = pass < numPasses - 1 ? BlendMode::opaque : BlendMode::alphaBlend;
			auto rpassType = blendMode == BlendMode::opaque ? RenderPassType::opaque : RenderPassType::blend;
			auto pipeline = getOrCreateRenderPipeline(rpassType,								//
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
