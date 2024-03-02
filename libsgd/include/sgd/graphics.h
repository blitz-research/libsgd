#ifndef SGD_GRAPHICS_H_INCLUDED
#define SGD_GRAPHICS_H_INCLUDED

#include "types.h"

// ***** Scene *****

SGD_API void SGD_DECL sgd_CreateScene();

SGD_API void SGD_DECL sgd_SetSceneAmbientLightColor(float red, float green, float blue, float alpha);

SGD_API void SGD_DECL sgd_SetSceneClearColor(float red, float green, float blue, float alpha);

SGD_API void SGD_DECL sgd_SetSceneClearDepth(float depth);

SGD_API void SGD_DECL sgd_SetSceneEnvTexture(SGD_Texture texture);

SGD_API void SGD_DECL sgd_RenderScene();

SGD_API void SGD_DECL sgd_Present();

// ***** Texture *****

#define SGD_TEXTURE_FORMAT_RGBA8        1
#define SGD_TEXTURE_FORMAT_SRGBA8       2
#define SGD_TEXTURE_FORMAT_RGBA16F      3
#define SGD_TEXTURE_FORMAT_DEPTH32F     4

#define SGD_TEXTURE_FLAGS_CLAMP_U       0x01
#define SGD_TEXTURE_FLAGS_CLAMP_V       0x02
#define SGD_TEXTURE_FLAGS_CLAMP_W       0x04
#define SGD_TEXTURE_FLAGS_CLAMP_COORDS  0x07
#define SGD_TEXTURE_FLAGS_FILTER        0x08
#define SGD_TEXTURE_FLAGS_MIPMAP        0x10
#define SGD_TEXTURE_FLAGS_CUBE          0x20
#define SGD_TEXTURE_FLAGS_CUBE_MIPMAP   0x30
#define SGD_TEXTURE_FLAGS_RENDER_TARGET 0x40

SGD_API SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags);

// ***** Material *****

SGD_API SGD_Material SGD_DECL sgd_LoadMaterial(SGD_String path);

// ***** Mesh *****

SGD_API SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path);

SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh mesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ, int uniform);

SGD_API void SGD_DECL sgd_TransformMeshTexCoords(SGD_Mesh mesh, float scaleX, float scaleY, float offsetX, float offsetY);

SGD_API SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, SGD_Material material);

SGD_API SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material);

// ***** Skybox *****

SGD_API SGD_Skybox SGD_DECL sgd_CreateSkybox();

SGD_API void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox skybox, SGD_Texture texture);

SGD_API void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox skybox, float roughness);

// ***** Model *****

SGD_API SGD_Model SGD_DECL sgd_CreateModel();

SGD_API void SGD_DECL sgd_SetModelMesh(SGD_Model model, SGD_Mesh mesh);

SGD_API void SGD_DECL sgd_SetModelColor(SGD_Model model, float red, float green, float blue, float alpha);

// ***** Light *****

SGD_API SGD_Light SGD_DECL sgd_CreateLight();

SGD_API void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);

SGD_API void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);

SGD_API void SGD_DECL sgd_SetLightFalloff(SGD_Light, float falloff);

// ***** Entity *****

SGD_API void SGD_DECL sgd_MoveEntity(SGD_Entity entity, float tx, float ty, float tz);

SGD_API void SGD_DECL sgd_TurnEntity(SGD_Entity entity, float rx, float ry, float rz);

#endif
