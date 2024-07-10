#include "image.h"

#include "../material/nullmaterial.h"

namespace sgd {

namespace {

struct alignas(16) ImageUniforms {
	Vec4f spriteRect;
	uint32_t spriteViewMode{};
};

auto shaderSource{
#include "imagerenderer.wgsl"
};

TexturePtr g_defaultTexture = new Texture({1, 1}, 1, TextureFormat::r8, TextureFlags::array | TextureFlags::clamp);

BindGroupDescriptor imageGeometryDesc( //
	"imageGeometry",				   //
	BindGroupType::geometry,		   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform),
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2DArray), //
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),									   //
	},
	shaderSource,							//
	{},										//
	wgpu::PrimitiveTopology::TriangleList); //

} // namespace

Image::Image() : m_material(new Material(&nullMaterialDescriptor)), m_bindGroup(new BindGroup(&imageGeometryDesc)) {

	m_material->cullMode = CullMode::none;

	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, nullptr, sizeof(ImageUniforms)));

	frames.changed.connect(nullptr, [=](Texture* ttexture) { //
		m_bindGroup->setTexture(1, ttexture);
		m_drawRect = {-drawHandle(), Vec2f(ttexture->size()) - drawHandle()};
	});
	frames = g_defaultTexture;

	blendMode.changed.connect(nullptr, [=](BlendMode mode) { //
		m_material->blendMode = mode;
	});
	blendMode = BlendMode::alphaBlend;

	spriteRect.changed.connect(nullptr, [=](CRectf) { //
		invalidate();
	});
	spriteRect.changed.emit(spriteRect());

	spriteViewMode.changed.connect(nullptr, [=](SpriteViewMode) { //
		invalidate();
	});
	spriteViewMode.changed.emit(spriteViewMode());

	drawHandle.changed.connect(nullptr, [=](CVec2f v) { //
		m_drawRect = {-v, Vec2f(frames()->size()) - v};
	});
	drawHandle.changed.emit(drawHandle());
}

void Image::onValidate() const {
	ImageUniforms uniforms;
	auto& r = spriteRect();
	uniforms.spriteRect = Vec4f(r.min.x, r.min.y, r.max.x - r.min.x, r.max.y - r.min.y);
	uniforms.spriteViewMode = (uint32_t)spriteViewMode();
	((Buffer*)m_bindGroup->getBuffer(0))->update(&uniforms, 0, sizeof(uniforms));
}

} // namespace sgd
