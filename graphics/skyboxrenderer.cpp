#include "skyboxrenderer.h"

#include "prelitmaterial.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/skyboxrenderer.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"skyboxRenderer", BindGroupType::renderer,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // SkyboxUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
	},
	{}, shaderSource);

} // namespace

SkyboxRenderer::SkyboxRenderer() {

	m_material = new Material(&prelitMaterialDescriptor);
	m_material->blendMode = BlendMode::opaque;
	m_material->depthFunc = DepthFunc::undefined;
	m_material->cullMode = CullMode::none;

	m_bindGroup = new BindGroup(&sgd::bindGroupDescriptor);

	SkyboxUniforms uniforms{};
	m_uniformBuffer = new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms));

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	addDependency(m_bindGroup, [=] {
		m_rebuildRenderOps = true;
		invalidate();
	});

	skyTexture.changed.connect(this, [=](Texture* texture) { //
		m_bindGroup->setTexture(1, texture);
	});

	worldMatrix.changed.connect(this, [=](CAffineMat4f) {
		m_updateUniforms = true;
		invalidate();
	});

	roughness.changed.connect(this, [=](float) {
		m_updateUniforms = true;
		invalidate();
	});
}

void SkyboxRenderer::onValidate(GraphicsContext* gc) const {

	if (m_rebuildRenderOps) {
		m_material->validate(gc);
		addRenderOp(gc, m_material, nullptr, nullptr, nullptr, m_bindGroup, DrawMode::triangleStrip, 4, 1, 0, false);
		m_rebuildRenderOps = false;
	}

	if (m_updateUniforms) {
		SkyboxUniforms uniforms;
		uniforms.worldMatrix = Mat4f(worldMatrix());
		uniforms.mipmapBias = roughness() * 15.99f;
		m_uniformBuffer->update(&uniforms, 0, sizeof(uniforms));
		m_updateUniforms = false;
	}
}

} // namespace sgd
