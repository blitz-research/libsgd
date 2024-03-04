#ifndef SGD_SGD_H_INCLUDED
#define SGD_SGD_H_INCLUDED

#if __cplusplus
#define SGD_EXTERN extern "C"
#else
#define SGD_EXTERN extern
#endif

#if SGD_DYNAMIC
#if SGD_EXPORT
#define SGD_API SGD_EXTERN __declspec(dllexport)
#else
#define SGD_API SGD_EXTERN __declspec(dllimport)
#endif
#else
#define SGD_API SGD_EXTERN
#endif

#if _MSC_VER && !_WIN64 // Cheeky hack for blitz3d support which needs __stdcall
#define SGD_DECL __stdcall
#elif _MSC_VER
#define SGD_DECL __cdecl
#else
#define SGD_DECL //__attribute__((__cdecl__)) - ignored?!?
#endif

typedef int SGD_Bool;
typedef int SGD_Handle;
typedef const char* SGD_String;

typedef SGD_Handle SGD_Texture;
typedef SGD_Handle SGD_Material;
typedef SGD_Handle SGD_Mesh;
typedef SGD_Handle SGD_Entity;
typedef SGD_Handle SGD_Camera;
typedef SGD_Handle SGD_Light;
typedef SGD_Handle SGD_Model;
typedef SGD_Handle SGD_Skybox;

// ***** System *****

// Returned by sgd_PollEvents.
#define SGD_EVENT_MASK_CLOSE_CLICKED 1
#define SGD_EVENT_MASK_SIZE_CHANGED 2

SGD_API void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL *handler)(const char* error, void* context), void* context);
SGD_API void SGD_DECL sgd_Run(void(SGD_DECL* start)());
SGD_API void SGD_DECL sgd_Error(SGD_String error);
SGD_API int SGD_DECL sgd_PollEvents();

// ***** Window *****

// Used by sgd_CreateWindow.
#define SGD_WINDOW_FLAGS_FULLSCREEN 1
#define SGD_WINDOW_FLAGS_RESIZABLE 2

SGD_API void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags);
SGD_API int SGD_DECL sgd_WindowWidth();
SGD_API int SGD_DECL sgd_WindowHeight();

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
SGD_API SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, SGD_Material material);
SGD_API SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material);
SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh mesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ, int uniform);
SGD_API void SGD_DECL sgd_TransformMeshTexCoords(SGD_Mesh mesh, float scaleX, float scaleY, float offsetX, float offsetY);

// ***** Skybox *****

SGD_API SGD_Skybox SGD_DECL sgd_CreateSkybox();
SGD_API void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox skybox, SGD_Texture texture);
SGD_API void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox skybox, float roughness);

// ***** Model *****

#define SGD_ANIMATION_MODE_ONESHOT 0
#define SGD_ANIMATION_MODE_LOOP 1
#define SGD_ANIMATION_MODE_PINGPONG 2

SGD_API SGD_Model SGD_DECL sgd_LoadModel(SGD_String path);
SGD_API SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path);
SGD_API SGD_Model SGD_DECL sgd_CreateModel();
SGD_API void SGD_DECL sgd_SetModelMesh(SGD_Model model, SGD_Mesh mesh);
SGD_API void SGD_DECL sgd_SetModelColor(SGD_Model model, float red, float green, float blue, float alpha);
SGD_API void SGD_DECL sgd_AnimateModel(SGD_Model model, int animation, float time, int mode);

// ***** Light *****

SGD_API SGD_Light SGD_DECL sgd_CreateLight();
SGD_API void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);
SGD_API void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);
SGD_API void SGD_DECL sgd_SetLightFalloff(SGD_Light, float falloff);

// ***** Entity *****

SGD_API SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity entity);
SGD_API void SGD_DECL sgd_SetEntityPosition(SGD_Entity entity, float tx, float ty, float tz);
SGD_API void SGD_DECL sgd_SetEntityRotation(SGD_Entity entity, float rx, float ry, float rz);
SGD_API void SGD_DECL sgd_MoveEntity(SGD_Entity entity, float tx, float ty, float tz);
SGD_API void SGD_DECL sgd_TurnEntity(SGD_Entity entity, float rx, float ry, float rz);
SGD_API void SGD_DECL sgd_TranslateEntity(SGD_Entity entity, float tx, float ty, float tz);
SGD_API void SGD_DECL sgd_RotateEntity(SGD_Entity entity, float tx, float ty, float tz);

#endif
