#include "fogeffect.h"

namespace sgd {

namespace {

struct alignas(16) FogEffectUniforms {
	Vec4f color;
	float near;
	float range;
	float power;
};

auto shaderSource{
#include "fogeffect.wgsl"
};

BindGroupDescriptor bindGroupDesc( //
	"fogEffect",				   //
	BindGroupType::geometry,	   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
		textureBindGroupLayoutEntry(3, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2D,wgpu::TextureSampleType::Depth),
		samplerBindGroupLayoutEntry(4, wgpu::ShaderStage::Fragment),
	},
	shaderSource, //
	{},			  //
	wgpu::PrimitiveTopology::TriangleStrip);

} // namespace

FogEffect::FogEffect() : m_bindGroup(new BindGroup(&bindGroupDesc)) {

	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, nullptr, sizeof(FogEffectUniforms)));
	m_bindGroup->setTexture(1, blackTexture());
	m_bindGroup->setTexture(3, dummyTexture(TextureFormat::depth32f));

	color.changed.connect(nullptr,[=](CVec4f){invalidate();});
	near.changed.connect(nullptr,[=](float){invalidate();});
	far.changed.connect(nullptr,[=](float){invalidate();});
	power.changed.connect(nullptr,[=](float){invalidate();});
}

Texture* FogEffect::onValidate(Texture* sourceTexture, Texture* depthBuffer) {

	m_renderTarget = getOrCreateRenderTarget(sourceTexture->size(), sourceTexture->format());

	FogEffectUniforms uniforms{color(), near(), far() - near(), power()};
	((Buffer*)m_bindGroup->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));
	m_bindGroup->setTexture(1, sourceTexture);
	m_bindGroup->setTexture(3, depthBuffer);

	return m_renderTarget;
}

void FogEffect::onRender(RenderContext* rc, BindGroup* sceneBindings) const {

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
