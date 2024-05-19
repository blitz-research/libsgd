#include "image.h"

#include "shaders/uniforms.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/imagematerial.wgsl"
};

TexturePtr g_defaultTexture = new Texture({1, 1}, 1, TextureFormat::r8, TextureFlags::array | TextureFlags::clamp);

BindGroupDescriptor bindGroupDescriptor( //
	"imageMaterial",
	1,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform),		   //
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment, wgpu::TextureViewDimension::e2DArray), //
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),									   //
	},
	{}, shaderSource);

const MaterialDescriptor materialDescriptor(									   //
	"image",																	   //
	&bindGroupDescriptor,														   //
	sizeof(ImageMaterialUniforms),												   //
	{																			   //
	 {"rect4f", {offsetof(ImageMaterialUniforms, rect), 4, new Rectf(-.5f, .5f)}}, //
	 {"spriteViewMode1i", {offsetof(ImageMaterialUniforms, spriteViewMode), 1, new uint32_t(1)}}},
	{
		{"frames", {1, nullptr}},
	});

} // namespace

Image::Image() : m_material(new Material(&materialDescriptor)) {

	frames.changed.connect(nullptr, [=](Texture* t) { //
		m_material->setTexture("frames", t);
		m_drawRect = {-drawHandle(), Vec2f(frames()->size()) - drawHandle()};
	});
	frames = g_defaultTexture;

	blendMode.changed.connect(nullptr, [=](BlendMode mode) { m_material->blendMode = mode; });
	blendMode = BlendMode::alpha;

	m_material->cullMode = CullMode::none;

	spriteRect.changed.connect(nullptr, [=](CRectf r) { //
		m_material->setVector4f("rect4f", Vec4f(r.min.x, r.min.y, r.max.x - r.min.x, r.max.y - r.min.y));
	});
	spriteRect.changed.emit(spriteRect());

	spriteViewMode.changed.connect(nullptr, [=](SpriteViewMode mode) { //
		m_material->setInt("spriteViewMode1i", (int)mode);
	});
	spriteViewMode.changed.emit(spriteViewMode());

	drawHandle.changed.connect(nullptr, [=](CVec2f v) { //
		m_drawRect = {-v, Vec2f(frames()->size()) - v};
	});
	drawHandle.changed.emit(drawHandle());
}

}; // namespace sgd
