#pragma once

#include "../core/bindgroup.h"

namespace sgd {

struct PlaneUniforms;

SGD_SHARED(Material);
SGD_SHARED(RenderQueue);

SGD_SHARED(PlaneBindings);

struct PlaneBindings : GraphicsResource {
	SGD_OBJECT_TYPE(PlaneBindings, GraphicsResource);

	PlaneBindings();

	Property<AffineMat4f> worldMatrix{};

	Property<Vec4f> color{Vec4f(1)};

	Property<MaterialPtr> material;

	void render(RenderQueue* rq) const;

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;
	MaterialPtr m_defaultMaterial;

	const PlaneUniforms& uniforms() const;
	PlaneUniforms& lockUniforms() const;
	void unlockUniforms() const;
};

}
