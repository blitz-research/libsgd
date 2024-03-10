#ifndef SGD_SGD_H_INCLUDED
#define SGD_SGD_H_INCLUDED

#include "keycodes.h"

//! @file

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

#define SGD_TRUE 1
#define SGD_FALSE 0

#define SGD_PI 3.14159265359f;
#define SGD_TWO_PI 6.28318530718f;
#define SGD_HALF_PI 1.5707963268f;
#define SGD_DEGREES_TO_RADIANS = .01745329252f;
#define SGD_RADIANS_TO_DEGREES = 57.295779513f;

// ***** System *****

// Returned by sgd_PollEvents.
#define SGD_EVENT_MASK_CLOSE_CLICKED 1
#define SGD_EVENT_MASK_SIZE_CHANGED 2

//! Set error handler callback.
SGD_API void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL* handler)(const char* error, void* context), void* context);

//! Run on new thread.
SGD_API void SGD_DECL sgd_Run(void(SGD_DECL* start)());

//! Generate runtime errror.
SGD_API void SGD_DECL sgd_Error(SGD_String error);

//! Poll system for events.
SGD_API int SGD_DECL sgd_PollEvents();

// ***** Window *****

// Used by sgd_CreateWindow.
#define SGD_WINDOW_FLAGS_FULLSCREEN 1
#define SGD_WINDOW_FLAGS_RESIZABLE 2

//! Create a new window.
SGD_API void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags);

//! Get window width.
SGD_API int SGD_DECL sgd_WindowWidth();

//! Get window height.
SGD_API int SGD_DECL sgd_WindowHeight();

//! True if key currently held down.
SGD_API SGD_Bool SGD_DECL sgd_KeyDown(int keyCode);

//! True if key pressed since last call to sgd_PollEvents().
SGD_API SGD_Bool SGD_DECL sgd_KeyHit(int keyCode);

//! Get next unicode character in key queue
SGD_API int SGD_DECL sgd_GetChar();

//! Mouse X position in window coordinates.
SGD_API float SGD_DECL sgd_MouseX();

//! Mouse Y position in window coordinates.
SGD_API float SGD_DECL sgd_MouseY();

//! Mouse scroll X value.
SGD_API float SGD_DECL sgd_MouseScrollX();

//! Mouse scroll Y value.
SGD_API float SGD_DECL sgd_MouseScrollY();

//! True if mouse button is curently held down.
SGD_API SGD_Bool SGD_DECL sgd_MouseButtonDown(int button);

//! True if mouse button is curently held down.
SGD_API SGD_Bool SGD_DECL sgd_MouseButtonHit(int button);

//! True if gamepad is currently connected.
SGD_API SGD_Bool SGD_DECL sgd_GamepadConnected(int gamepad);

//! True if gamepad button currently held down.
SGD_API SGD_Bool SGD_DECL sgd_GamepadButtonDown(int gamepad, int button);

//! True if gamepad button pressed since last call to sgd_PollEvents().
SGD_API SGD_Bool SGD_DECL sgd_GamepadButtonHit(int gamepad, int button);

//! Value in the range -1 to 1 representing joystick axis position.
SGD_API float SGD_DECL sgd_GamepadAxis(int gamepad, int axis);

// ***** Scene *****

//! Create a new scene.
SGD_API void SGD_DECL sgd_CreateScene();

//! Set scene ambient light color.
SGD_API void SGD_DECL sgd_SetSceneAmbientLightColor(float red, float green, float blue, float alpha);

//! Set scene clear color.
SGD_API void SGD_DECL sgd_SetSceneClearColor(float red, float green, float blue, float alpha);

//! Set scene clear depth.
SGD_API void SGD_DECL sgd_SetSceneClearDepth(float depth);

//! Set scene environment texture.
SGD_API void SGD_DECL sgd_SetSceneEnvTexture(SGD_Texture texture);

//! Render scene.
SGD_API void SGD_DECL sgd_RenderScene();

//! Present.
SGD_API void SGD_DECL sgd_Present();

// ***** Texture *****

#define SGD_TEXTURE_FORMAT_RGBA8 1
#define SGD_TEXTURE_FORMAT_SRGBA8 2
#define SGD_TEXTURE_FORMAT_RGBA16F 3
#define SGD_TEXTURE_FORMAT_DEPTH32F 4

//! Texture flag constants
#define SGD_TEXTURE_FLAGS_CLAMP_U 0x01
#define SGD_TEXTURE_FLAGS_CLAMP_V 0x02
#define SGD_TEXTURE_FLAGS_CLAMP_W 0x04
#define SGD_TEXTURE_FLAGS_CLAMP_COORDS 0x07
#define SGD_TEXTURE_FLAGS_FILTER 0x08
#define SGD_TEXTURE_FLAGS_MIPMAP 0x10
#define SGD_TEXTURE_FLAGS_CUBE 0x20
#define SGD_TEXTURE_FLAGS_CUBE_MIPMAP 0x30
#define SGD_TEXTURE_FLAGS_RENDER_TARGET 0x40

//! Load a new texture.
SGD_API SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags);

// ***** Material *****

//! Load a new material.
SGD_API SGD_Material SGD_DECL sgd_LoadMaterial(SGD_String path);

// ***** Mesh *****

//! Load a new mesh.
SGD_API SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path);

//! Create a new mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
											SGD_Material material);

//! Create a new mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material);

//! Create a new mesh.
SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh mesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
								  SGD_Bool uniform);

//! Create a new mesh.
SGD_API void SGD_DECL sgd_TransformMeshTexCoords(SGD_Mesh mesh, float scaleX, float scaleY, float offsetX, float offsetY);

// ***** Skybox *****

//! Create a new skybox.
SGD_API SGD_Skybox SGD_DECL sgd_CreateSkybox();

//! Set skybox texture.
SGD_API void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox skybox, SGD_Texture texture);

//! Set skybox roughness in the ranmge 0 to 1.
SGD_API void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox skybox, float roughness);

// ***** Model *****

// Used by sgd_AnimateModel
#define SGD_ANIMATION_MODE_ONESHOT 0
#define SGD_ANIMATION_MODE_LOOP 1
#define SGD_ANIMATION_MODE_PINGPONG 2

//! Load a new model.
SGD_API SGD_Model SGD_DECL sgd_LoadModel(SGD_String path);

//! Load a boned model.
SGD_API SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path, SGD_Bool skinned);

SGD_API SGD_Model SGD_DECL sgd_CreateModel();
SGD_API void SGD_DECL sgd_SetModelMesh(SGD_Model model, SGD_Mesh mesh);
SGD_API void SGD_DECL sgd_SetModelColor(SGD_Model model, float red, float green, float blue, float alpha);
SGD_API void SGD_DECL sgd_AnimateModel(SGD_Model model, int animation, float time, int mode);

// ***** Camera *****

SGD_API SGD_Camera SGD_DECL sgd_CreatePerspectiveCamera();
SGD_API SGD_Camera SGD_DECL sgd_CreateOrthographicCamera();
SGD_API void SGD_DECL sgd_SetCameraFovY(SGD_Camera camera, float fovY);
SGD_API void SGD_DECL sgd_SetCameraNear(SGD_Camera camera, float near);
SGD_API void SGD_DECL sgd_SetCameraFar(SGD_Camera camera, float far);

// ***** Light *****

SGD_API SGD_Light SGD_DECL sgd_CreateDirectionalLight();
SGD_API SGD_Light SGD_DECL sgd_CreatePointLight();
SGD_API SGD_Light SGD_DECL sgd_CreateSpotLight();
SGD_API void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);
SGD_API void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);
SGD_API void SGD_DECL sgd_SetLightFalloff(SGD_Light light, float falloff);
SGD_API void SGD_DECL sgd_SetLightInnerConeAngle(SGD_Light light, float angle);
SGD_API void SGD_DECL sgd_SetLightOuterConeAngle(SGD_Light light, float angle);

// ***** Entity *****

SGD_API SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity entity);
SGD_API void SGD_DECL sgd_SetEntityPosition(SGD_Entity entity, float tx, float ty, float tz);
SGD_API void SGD_DECL sgd_SetEntityRotation(SGD_Entity entity, float rx, float ry, float rz);
SGD_API void SGD_DECL sgd_MoveEntity(SGD_Entity entity, float tx, float ty, float tz);
SGD_API void SGD_DECL sgd_TurnEntity(SGD_Entity entity, float rx, float ry, float rz);
SGD_API void SGD_DECL sgd_TranslateEntity(SGD_Entity entity, float tx, float ty, float tz);
SGD_API void SGD_DECL sgd_RotateEntity(SGD_Entity entity, float tx, float ty, float tz);

#endif
