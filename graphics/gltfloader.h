#pragma once

#include "mesh.h"

namespace sgd {

Expected<Mesh*, FileioEx> loadGLTFMesh(CPath path);

}
