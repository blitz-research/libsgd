#pragma once

#include "material.h"

namespace sgd {

Expected<Material*, FileioEx> loadMaterial(CPath path);

Material* rgbaMaterial(uint32_t rgba, float metallic, float roughness);

}
