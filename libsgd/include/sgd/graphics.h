#ifndef SGD_GRAPHICS_H_INCLUDED
#define SGD_GRAPHICS_H_INCLUDED

#include "types.h"

// ***** Scene *****

SGD_EXTERN void SGD_DECL sgd_CreateScene();

SGD_EXTERN void SGD_DECL sgd_SetSceneAmbientLightColor(float red, float green, float blue, float alpha);

SGD_EXTERN void SGD_DECL sgd_SetSceneClearColor(float red, float green, float blue, float alpha);

SGD_EXTERN void SGD_DECL sgd_SetSceneClearDepth(float depth);

SGD_EXTERN void SGD_DECL sgd_SetSceneEnvTexture(SGD_Texture texture);

SGD_EXTERN void SGD_DECL sgd_RenderScene();

SGD_EXTERN void SGD_DECL sgd_Present();

// ***** Texture *****

#define SGD_TEXTURE_FORMAT SGD_TEXTURE_FORMAT_SRGBA	2

SGD_EXTERN SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags);

// ***** Material *****

SGD_EXTERN SGD_Material SGD_DECL sgd_LoadMaterial(SGD_String path);

// ***** Mesh *****

SGD_EXTERN SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, SGD_Material material);

SGD_EXTERN SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material);

SGD_EXTERN void SGD_DECL sgd_TransformMeshTexCoords(SGD_Mesh mesh, float scaleX, float scaleY, float offsetX, float offsetY);

// ***** Skybox *****

SGD_EXTERN SGD_Skybox SGD_DECL sgd_CreateSkybox();

SGD_EXTERN void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox skybox, SGD_Texture texture);

// ***** Model *****

SGD_EXTERN SGD_Model SGD_DECL sgd_CreateModel();

SGD_EXTERN void SGD_DECL sgd_SetModelMesh(SGD_Model model, SGD_Mesh mesh);

SGD_EXTERN void SGD_DECL sgd_SetModelColor(SGD_Model model, float red, float green, float blue, float alpha);

// ***** Light *****

SGD_EXTERN SGD_Light SGD_DECL sgd_CreateLight();

SGD_EXTERN void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);

SGD_EXTERN void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);

SGD_EXTERN void SGD_DECL sgd_SetLightFalloff(SGD_Light, float falloff);

// ***** Entity *****

SGD_EXTERN void SGD_DECL sgd_MoveEntity(SGD_Entity entity, float tx, float ty, float tz);

SGD_EXTERN void SGD_DECL sgd_TurnEntity(SGD_Entity entity, float rx, float ry, float rz);

#endif
