#pragma once

#include "material.h"

namespace sgd {

Material* rgbaMaterial(uint32_t rgba, float metallic, float roughness);

Material* loadMaterial(CPath path);

}
