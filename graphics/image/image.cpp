#include "image.h"

#include "imagematerial.h"

namespace sgd {

Image::Image(Texture* texture)
	: m_texture(texture),																				 //
	  m_material(new Material(bool(texture->flags() & TextureFlags::array) ? &imageMaterialDescriptorMX	 //
																		   : &imageMaterialDescriptor)), //
	  m_bindGroup(new BindGroup(&imageBindGroupDescriptor)),											 //
	  m_uniforms(new Buffer(BufferType::uniform, nullptr, sizeof(ImageUniforms))) {

	m_material->setTexture("albedoTexture", texture);
	m_material->blendMode = blendMode();
	m_material->depthFunc = DepthFunc::lessEqual;
	m_material->cullMode = CullMode::none;

	blendMode.changed.connect(nullptr, [=](BlendMode b) { //
		m_material->blendMode = b;
	});

	m_bindGroup->setBuffer(0, m_uniforms);

	lockUniforms() = {};
	unlockUniforms();

	viewMode.changed.connect(nullptr, [=](ImageViewMode m) { //
		lockUniforms().viewMode = (uint32_t)m;
		unlockUniforms();
	});
	viewMode.changed.emit(viewMode());

	rect.changed.connect(nullptr, [=](CRectf r) { //
		lockUniforms().rect = {r.min.x, r.min.y, r.max.x - r.min.x, r.max.y - r.min.y};
		unlockUniforms();
	});
	rect.changed.emit(rect());
}

const ImageUniforms& Image::uniforms() const {
	return *(ImageUniforms*)m_uniforms->data();
}

ImageUniforms& Image::lockUniforms() {
	return *(ImageUniforms*)m_uniforms->lock(0, sizeof(ImageUniforms));
}

void Image::unlockUniforms() {
	m_uniforms->unlock();
}

} // namespace sgd
