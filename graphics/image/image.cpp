#include "image.h"

#include "../material/nullmaterial.h"

namespace sgd {

namespace {

struct alignas(16) ImageGeometryUniforms {
	Vec4f rect;
	uint32_t viewMode{};
};

BindGroupDescriptor bindGroupDesc( //
	"imageGeometry",			   //
	BindGroupType::geometry,	   //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform),
	},
	"",										//
	{},										//
	wgpu::PrimitiveTopology::TriangleList); //

} // namespace

Image::Image(CMaterial* material)
	: m_material(material),																	//
	  m_bindGroup(new BindGroup(&bindGroupDesc)),											//
	  m_uniforms(new Buffer(BufferType::uniform, nullptr, sizeof(ImageGeometryUniforms))) { //

	m_bindGroup->setBuffer(0, m_uniforms);

	rect.changed.connect(nullptr, [=](CRectf) { //
		invalidate();
	});

	viewMode.changed.connect(nullptr, [=](ImageViewMode) { //
		invalidate();
	});
}

void Image::onValidate() const {
	auto& uniforms = *(ImageGeometryUniforms*)m_uniforms->lock(0, sizeof(ImageGeometryUniforms));
	auto& r = rect();
	uniforms.rect = Vec4f(r.min.x, r.min.y, r.max.x - r.min.x, r.max.y - r.min.y);
	uniforms.viewMode = (uint32_t)viewMode();
	m_uniforms->unlock();
}

} // namespace sgd
