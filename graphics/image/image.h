#pragma once

#include "imagebindgroup.h"

#include "../material/material.h"

namespace sgd {

enum struct ImageViewMode {
	fixed = 1,	 // locked to camera, 2d style
	free = 2,	 // free of camera, billboard style
	upright = 3, // upright, tree style
};

SGD_SHARED(Image);

struct Image : GraphicsResource {
	SGD_OBJECT_TYPE(Image, GraphicsResource);

	explicit Image(Texture* texture);

	Property<ImageViewMode> viewMode{ImageViewMode::fixed};

	Property<Rectf> rect{Rectf(-.5f, .5f)};

	Property<BlendMode> blendMode{BlendMode::alphaBlend};

	CBindGroup* bindGroup() const {
		return m_bindGroup;
	}

	Material* material() const {
		return m_material;
	}

	Texture* texture() const {
		return m_texture;
	}

private:
	TexturePtr m_texture;
	MaterialPtr m_material;
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniforms;

	const ImageUniforms& uniforms() const;
	ImageUniforms& lockUniforms();
	void unlockUniforms();
};

} // namespace sgd
