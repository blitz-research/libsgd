#include "blureffect.h"

#include "gaussianfilter.h"

namespace sgd {

namespace {

struct alignas(16) BlurEffectUniforms {
	Vec2f texCoordScale;
	uint32_t kernelSize{};
	uint32_t padding{};
	Vec4f kernel[64];
};

auto shaderSource{
#include "blureffect.wgsl"
};

BindGroupDescriptor bindGroupDesc( //
	"blurEffect",				   //
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

BlurEffect::BlurEffect() {
	for (auto& bindGroup : m_bindGroups) {
		bindGroup = new BindGroup(&bindGroupDesc);
		bindGroup->setBuffer(0, new Buffer(BufferType::uniform, nullptr, sizeof(BlurEffectUniforms)));
		bindGroup->setTexture(1,dummyTexture(TextureFormat::rgba8,TextureFlags::none));
	}

	radius.changed.connect(nullptr, [=](uint32_t) {
		invalidate();
	});
}

Texture* BlurEffect::onValidate(Texture* sourceTexture, Texture* depthBuffer) {

	m_renderTargets[0] = getOrCreateRenderTarget(sourceTexture->size(), sourceTexture->format());
	m_renderTargets[1] = sourceTexture;

	auto r = std::min(std::max(this->radius(), 1u), 31u);
	auto kernelSize = r * 2 + 1;
	// https://dsp.stackexchange.com/a/74157
	float sigma = (float)r / 4;	//Should be 2 according to above, but 4 Looks much better in bloom, all just a blur in blur.
	auto kernel = generateKernel(kernelSize, sigma);
	BlurEffectUniforms uniforms{};
	uniforms.kernelSize = kernelSize;
	std::memcpy(&uniforms.kernel, kernel.data(), sizeof(Vec4f) * kernelSize);

	uniforms.texCoordScale = Vec2f(1.0f / (float)sourceTexture->size().x, 0.0f);
	((Buffer*)m_bindGroups[0]->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));
	m_bindGroups[0]->setTexture(1, sourceTexture);

	uniforms.texCoordScale = Vec2f(0.0f, 1.0f / (float)sourceTexture->size().y);
	((Buffer*)m_bindGroups[1]->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));
	m_bindGroups[1]->setTexture(1, m_renderTargets[0]);

	m_pipeline = {};

	return m_renderTargets[1];
}

void BlurEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

	if (!m_pipeline) {
		m_pipeline = getOrCreateRenderPipeline(		  //
			RenderPassType::opaque,					  //
			BlendMode::opaque,						  //
			DepthFunc::undefined,					  //
			CullMode::none,							  //
			emptyBindGroup(BindGroupType::material),  //
			m_bindGroups[0],						  //
			emptyBindGroup(BindGroupType::renderer)); //
	}

	for (int i = 0; i < 2; ++i) {

		auto pass = rc->beginRenderPass(RenderPassType::opaque, m_renderTargets[i], nullptr, {}, 1, sceneBindings);

		pass.SetBindGroup(1, emptyBindGroup(BindGroupType::material)->wgpuBindGroup());
		pass.SetBindGroup(2, m_bindGroups[i]->wgpuBindGroup());
		pass.SetBindGroup(3, emptyBindGroup(BindGroupType::renderer)->wgpuBindGroup());
		pass.SetPipeline(m_pipeline);
		pass.Draw(4);

		rc->endRenderPass();
	}
}

} // namespace sgd
