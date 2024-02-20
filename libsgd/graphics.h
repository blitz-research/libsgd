#ifndef SGD_GRAPHICS_H_INCLUDED
#define SGD_GRAPHICS_H_INCLUDED

#include "types.h"

// ***** Scene *****

SGD_API void sgd_CreateScene();

SGD_API void sgd_SetSceneAmbientColor(float red, float green, float blue, float alpha);

SGD_API void sgd_SetSceneClearColor(float red, float green, float blue, float alpha);

SGD_API void sgd_SetSceneClearDepth(float depth);

// ***** Material *****

SGD_API SGD_Material sgd_LoadMaterial(SGD_String path);

// ***** Mesh *****

SGD_API SGD_Mesh sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, SGD_Material material);

SGD_API SGD_Mesh sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material);

// ***** Model *****

SGD_API SGD_Mesh sgd_CreateModel(SGD_Mesh mesh);

// ***** Entity *****

SGD_API void sgd_MoveEntity(SGD_Entity entity, float x,float y, float z);

#endif
