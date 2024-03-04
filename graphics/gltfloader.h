#pragma once

#include "mesh.h"
#include "model.h"

namespace sgd {

Expected<Mesh*, FileioEx> loadGLTFMesh(CPath path);

Expected<Model*, FileioEx> loadGLTFModel(CPath path);

}
