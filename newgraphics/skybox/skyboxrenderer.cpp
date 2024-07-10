#include "skyboxrenderer.h"

#include "../material/nullmaterial.h"
#include "../render/rendercontext.h"

namespace sgd {

namespace {

struct alignas(16) SkyboxUniforms {
	Mat4f worldMatrix;
	float mipmapBias{0};
};

auto shaderSource{
#include "skyboxrenderer.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"skyboxGeometry", BindGroupType::geometry,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // SkyboxUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::Cube),
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),
	},
	shaderSource, {}, wgpu::PrimitiveTopology::TriangleStrip);

} // namespace

SkyboxRenderer::SkyboxRenderer() {

	// Clear material
	m_material = new Material(&nullMaterialDescriptor);
	m_material->blendMode = BlendMode::opaque;
	m_material->depthFunc = DepthFunc::undefined;
	m_material->cullMode = CullMode::none;

	SkyboxUniforms uniforms{};
	m_uniformBuffer = new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms));

	m_bindGroup = new BindGroup(&sgd::bindGroupDescriptor);
	m_bindGroup->setBuffer(0, m_uniformBuffer);

	skyTexture.changed.connect(this, [=](Texture* texture) { //
		m_bindGroup->setTexture(1, texture);
	});

	worldMatrix.changed.connect(this, [=](CAffineMat4f) {
		invalidate();
	});

	roughness.changed.connect(this, [=](float) {
		invalidate();
	});
}

void SkyboxRenderer::onValidate() const {
	SkyboxUniforms uniforms;
	uniforms.worldMatrix = Mat4f(worldMatrix());
	uniforms.mipmapBias = roughness() * 15.99f;
	m_uniformBuffer->update(&uniforms, 0, sizeof(uniforms));
}

void SkyboxRenderer::render(RenderContext* rc) const {
	if (rc->renderPassType() != RenderPassType::opaque) return;

	rc->setVertexBuffer(nullptr);
	rc->setIndexBuffer(nullptr);
	rc->setMaterial(m_material);
	rc->setGeometry(m_bindGroup);
	rc->setRenderer(emptyBindGroup(BindGroupType::renderer));
	rc->render(4, 1, 0);
}

} // namespace sgd
