#pragma once

#include "material.h"

namespace sgd {

Expected<Material*, FileioEx> loadMaterial(CPath path);

Material* createPBRMaterial();
Expected<Material*, FileioEx> loadPBRMaterial(CPath path);

Material* creatEmissiveMaterial();
Expected<Material*, FileioEx> loadEmissiveMaterial(CPath path);

Material* createPrelitMaterial();
Expected<Material*, FileioEx> loadPrelitMaterial(CPath path);

}
