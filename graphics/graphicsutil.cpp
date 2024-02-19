#include "graphicsutil.h"
#include "bindgroup.h"

#include "shaders/uniforms.h"

namespace sgd {

void setCameraUniforms(GraphicsContext* gc, const CameraUniforms& uniforms) {
	gc->bindGroup0()->getBuffer(0)->update(&uniforms, 0, sizeof(uniforms));
}

void setLightingUniforms(GraphicsContext* gc, CAffineMat4f matrix) {
}

} // namespace sgd
