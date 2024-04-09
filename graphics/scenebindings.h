#pragma once

#include "bindgroup.h"
#include "shaders/uniforms.h"

// Should maybe be Environment?

namespace sgd {

SGD_SHARED(SceneBindings);

extern const BindGroupDescriptor sceneBindingsDescriptor;

struct SceneBindings : GraphicsResource {
	SGD_OBJECT_TYPE(SceneBindings, GraphicsResource);

	SceneBindings();

	void updateCameraUniforms(const CameraUniforms& uniforms);

	const CameraUniforms& cameraUniforms() const {
		return *(const CameraUniforms*)m_cameraUniforms->data();
	}

	void updateLightingUniforms(const LightingUniforms& uniforms);

	const LightingUniforms& lightingUniforms() const {
		return *(const LightingUniforms*)m_lightingUniforms->data();
	}

	void setEnvTexture(const Texture* texture);

	void setPointShadowTexture(const Texture* texture);

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

private:
	BufferPtr m_cameraUniforms;
	BufferPtr m_lightingUniforms;
	BindGroupPtr m_bindGroup;
};

} // namespace sgd
