#pragma once

#include "mesh.h"

namespace sgd {

Boxf bounds(CMesh* mesh);

void transform(Mesh* mesh, CAffineMat4f matrix);

void transformTexCoords(Mesh* mesh, CVec2f scale, CVec2f offset);

void fit(Mesh* mesh, CBoxf box, bool uniform);

void updateNormals(Mesh* mesh);

void updateTangents(Mesh* mesh);

}
