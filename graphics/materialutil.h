#pragma once

#include "material.h"

namespace sgd {

Material* createPBRMaterial(CVec4f albedoColor = Vec4f{1});
Expected<Material*, FileioEx> loadPBRMaterial(CPath path);

Material* createPrelitMaterial(CVec4f albedoColor = Vec4f{1});
Expected<Material*, FileioEx> loadPrelitMaterial(CPath path);

}
