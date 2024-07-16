#pragma once

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

	explicit Image(CMaterial* material);

	Property<ImageViewMode> viewMode{ImageViewMode::fixed};
	Property<Rectf> rect{Rectf(-.5f, .5f)};
	Property<Vec2f> handle{};

	CMaterial* material()const{
		return m_material;
	}

	CBindGroup* bindGroup() const {
		return m_bindGroup;
	}

private:
	CMaterialPtr m_material;
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniforms;

	void onValidate() const override;
};

} // namespace sgd
