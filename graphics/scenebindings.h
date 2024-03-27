#pragma once

#include "bindgroup.h"

namespace sgd {

SGD_SHARED(SceneBindings);

struct SceneBindings : GraphicsResource {
	SGD_OBJECT_TYPE(SceneBindings, GraphicsResource);

	SceneBindings();

	Buffer* cameraUniforms() {
		return m_cameraUniforms;
	}

	Buffer* lightingUniforms() {
		return m_lightingUniforms;
	}

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_cameraUniforms;
	BufferPtr m_lightingUniforms;
};

}
