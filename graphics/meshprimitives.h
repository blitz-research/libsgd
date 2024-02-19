#pragma once

#include "mesh.h"

namespace sgd {

Mesh* createBoxMesh(CBoxf box, Material* material);

Mesh* createSphereMesh(float radius, uint32_t xSegs, uint32_t ySegs, Material* material);

Mesh* createCylinderMesh(float length, float radius, uint32_t segs, Material* material);

Mesh* createConeMesh(GraphicsContext* gc,float length, float radius, uint32_t segs, Material* material);

Mesh* createTorusMesh(float outerRadius, float innerRadius, uint32_t outerSegs, uint32_t innerSegs, Material* material);

}
