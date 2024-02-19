#pragma once

#include "graphics.h"

#include "shaders/uniforms.h"

namespace sgd {

void setCameraUniforms(GraphicsContext* gc, const CameraUniforms& uniforms);

void setLightingUniforms(GraphicsContext* gc, const LightingUniforms& uniforms);

void setEnvTexture(GraphicsContext* gc, CTexture* texture);

}
