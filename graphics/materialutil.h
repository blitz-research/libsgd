#pragma once

#include "material.h"

namespace sgd {

Expected<Material*, FileioEx> loadMaterial(CPath path);

Material* rgbaMaterial(uint32_t rgba, float metallic=0, float roughness=1);

Material* colorMaterial(CVec4f color, float metallic=0, float roughness=1);

}
