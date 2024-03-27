#pragma once

#include "material.h"

namespace sgd {

Material* createPBRMaterial();
Expected<Material*, FileioEx> loadPBRMaterial(CPath path);

Material* createMatteMaterial();
Expected<Material*, FileioEx> loadMatteMaterial(CPath path);

Material* createSpriteMaterial();
Expected<Material*, FileioEx> loadSpriteMaterial(CPath path);

}
