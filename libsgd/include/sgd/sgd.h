#ifndef SGD_SGD_H_INCLUDED
#define SGD_SGD_H_INCLUDED

#include "keycodes.h"

//! @file

//! @cond

#if __cplusplus
#define SGD_EXTERN extern "C"
#else
#define SGD_EXTERN extern
#endif

#if SGD_DYNAMIC && _WIN32
#if SGD_EXPORT
#define SGD_API SGD_EXTERN __declspec(dllexport)
#else
#define SGD_API SGD_EXTERN __declspec(dllimport)
#endif
#else
#define SGD_API SGD_EXTERN
#endif

#if _MSC_VER && !_WIN64 // Cheeky hack for 32 bit blitz3d support which needs __stdcall
#define SGD_DECL __stdcall
#elif _MSC_VER
#define SGD_DECL __cdecl
#else
#define SGD_DECL //__attribute__((__cdecl__)) - ignored?!?
#endif

#define SGD_TRUE 1
#define SGD_FALSE 0

#define SGD_PI 3.14159265359f
#define SGD_TWO_PI 6.28318530718f
#define SGD_HALF_PI 1.5707963268f
#define SGD_DEGREES_TO_RADIANS .01745329252f
#define SGD_RADIANS_TO_DEGREES 57.295779513f

//! @endcond

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
typedef SGD_Handle SGD_Sprite;
typedef SGD_Handle SGD_Skybox;

// ***** System *****

//! @cond Event type constants returned by sgd_PollEvents.
#define SGD_EVENT_MASK_CLOSE_CLICKED 1
#define SGD_EVENT_MASK_SIZE_CHANGED 2
//! @endcond

//! Start up libsgd.
SGD_API void SGD_DECL sgd_Startup();

//! Shut down libsgd.
SGD_API void SGD_DECL sgd_Shutdown();

//! Set error handler callback.
SGD_API void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL* handler)(const char* error, void* context), void* context);

//! Generate runtime error.
SGD_API void SGD_DECL sgd_Error(SGD_String error);

//! Generate a modal alert dialog.
SGD_API void SGD_DECL sgd_Alert(SGD_String message);

//! Poll system for events.
//!
//! @The returned value can contain one or more of the following bitmasks:
//!
//! Event mask                   | Integer value | Description
//!------------------------------|---------------|------------
//! SGD_EVENT_MASK_CLOSE_CLICKED | 1             | Window close button was clicked.
//! SGD_EVENT_MASK_SIZE_CHANGED  | 2             | Window was resized.
SGD_API int SGD_DECL sgd_PollEvents();

// ***** Window *****

//! @cond Window flags for use with CreateWindow.
#define SGD_WINDOW_FLAGS_FULLSCREEN 1
#define SGD_WINDOW_FLAGS_RESIZABLE 2
//! @endcond

//! Create a new window.
//!
//! @flags should be one or more of the following bit mask values:
//!
//! Window flag                 | Integer value | Description
//! ----------------------------|---------------|------------
//! SGD_WINDOW_FLAGS_FULLSCREEN | 1             | Create a fullscreen window.
//! SGD_WINDOW_FLAGS_RESIZABLE  | 2             | Create a resizable window.
SGD_API void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags);

//! Destroy window.
SGD_API void SGD_DECL sgd_DestroyWindow();

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

//! Set WebGPU backend: D3D12, D3D11, Vulkan
SGD_API void SGD_DECL sgd_SetWebGPUBackend(SGD_String backend);

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

//! @cond Texture format constants
#define SGD_TEXTURE_FORMAT_RGBA8 1
#define SGD_TEXTURE_FORMAT_SRGBA8 2
#define SGD_TEXTURE_FORMAT_RGBA16F 3
#define SGD_TEXTURE_FORMAT_DEPTH32F 4
//! @endcond

//! @cond Texture flag constants
#define SGD_TEXTURE_FLAGS_NONE 0x0
#define SGD_TEXTURE_FLAGS_CLAMP_U 0x01
#define SGD_TEXTURE_FLAGS_CLAMP_V 0x02
#define SGD_TEXTURE_FLAGS_CLAMP_W 0x04
#define SGD_TEXTURE_FLAGS_CLAMP_COORDS 0x07
#define SGD_TEXTURE_FLAGS_FILTER 0x08
#define SGD_TEXTURE_FLAGS_MIPMAP 0x10
#define SGD_TEXTURE_FLAGS_CUBE 0x20
#define SGD_TEXTURE_FLAGS_CUBE_MIPMAP 0x30
#define SGD_TEXTURE_FLAGS_RENDER_TARGET 0x40
//! @endcond

//! Load a new texture.
//!
//! @param `path` is the file path of the texture image to load.
//!
//! @param `format` should be one of the following:
//!
//! Texture format             | Integer value | Bytes per texel | Description
//! ---------------------------|---------------|-----------------|------------
//! SGD_TEXTURE_FORMAT_RGBA8   | 1             | 4               | Unsigned byte per component linear red/green/blue/alpha
//! SGD_TEXTURE_FORMAT_SRGBA8  | 2             | 4               | Unsigned byte per component standard red/green/blue/alpha
//! SGD_TEXTURE_FORMAT_RGBA16F | 3             | 8               | 16 bit float per component linear red/green/blue/alpha
//! SGD_TEXTURE_FORMAT_DEPTH32F | 4            | 4               | 32 bit float per component depth
//!
//! Note that you should generally use the 'standard' formats when dealing with WYSIWG images,
//! 'linear' when dealing with algorithmically generated data.
//!
//! @param `flags` should be one or more of the following bitmasks:
//!
//! Texture flags             | Integer value | Description
//! --------------------------|---------------|------------
//! SGD_TEXTURE_FLAGS_NONE    | 0             | No texture flags.
//! SGD_TEXTURE_FLAGS_CLAMP_U | 1             | Clamp texture U coordinate.
//! SGD_TEXTURE_FLAGS_CLAMP_V | 2             | Clamp texture V coordinate.
//! SGD_TEXTURE_FLAGS_CLAMP_W | 4             | Clamp texture W coordinate.
//! SGD_TEXTURE_FLAGS_FILTER  | 8             | Filter magnified texels.
//! SGD_TEXTURE_FLAGS_MIPMAP  | 16            | Mipmap minified texels.
//! SGD_TEXTURE_FLAGS_CUBE    | 32            | Create a cube texture.
//! SGD_TEXTURE_RENDER_TARGET | 64            | Create a texture that can be rendered to.
SGD_API SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags);

// ***** Material *****

//! @cond blend mode constants
#define SGD_BLEND_MODE_OPAQUE   1
#define SGD_BLEND_MODE_ALPHA    2
#define SGD_BLEND_MODE_ADDITIVE 3
#define SGD_BLEND_MODE_MULTIPLY 4
//! @endcond

//! @cond depth func constants
#define SGD_DEPTH_FUNC_NEVER         1
#define SGD_DEPTH_FUNC_LESS          2
#define SGD_DEPTH_FUNC_EQUAL         3
#define SGD_DEPTH_FUNC_LESS_EQUAL    4
#define SGD_DEPTH_FUNC_GREATER       5
#define SGD_DEPTH_FUNC_NOT_EQUAL     6
#define SGD_DEPTH_FUNC_GREATER_EQUAL 7
#define SGD_DEPTH_FUNC_ALWAYS        8
//! @endcond

//! @cond cull mode constants
#define SGD_CULL_MODE_NONE  1
#define SGD_CULL_MODE_FRONT 2
#define SGD_CULL_MODE_BACK  3
//! @endcond

//!

//! Create a new PBR material.
SGD_API SGD_Material SGD_DECL sgd_CreatePBRMaterial();

//! Load a new PBR material.
SGD_API SGD_Material SGD_DECL sgd_LoadPBRMaterial(SGD_String path);

//! Create a new matte matterial.
SGD_API SGD_Material SGD_DECL sgd_CreateMatteMaterial();

//! Load a new matte material.
SGD_API SGD_Material SGD_DECL sgd_LoadMatteMaterial(SGD_String path);

//! Create a new sprite matterial.
SGD_API SGD_Material SGD_DECL sgd_CreateSpriteMaterial();

//! Load a new sprite material.
SGD_API SGD_Material SGD_DECL sgd_LoadSpriteMaterial(SGD_String path);

//! Set material blend mode.
//!
//! `blendMode` should be one of the following:
//!
//! Blend mode              | Integer value | Description
//! ------------------------|---------------|------------
//! SGD_BLEND_MODE_OPAQUE   | 1             | Opaque
//! SGD_BLEND_MODE_ALPHA    | 2             | Alpha blended
//! SGD_BLEND_MODE_ADDITIVE | 3             | Additive blended
//! SGD_BLEND_MODE_MULTIPLY | 4             | Multiply blended
SGD_API void SGD_DECL sgd_SetMaterialBlendMode(SGD_Material material, int blendMode);

//! Set material depth comparison function.
//!
//! `depthFunc` should be one of the following:
//!
//! Blend mode                   | Integer value
//! -----------------------------|--------------
//! SGD_DEPTH_FUNC_NEVER         | 1
//! SGD_DEPTH_FUNC_LESS          | 2
//! SGD_DEPTH_FUNC_EQUAL         | 3
//! SGD_DEPTH_FUNC_LESS_EQUAL    | 4
//! SGD_DEPTH_FUNC_GREATER       | 5
//! SGD_DEPTH_FUNC_NOT_EQUAL     | 6
//! SGD_DEPTH_FUNC_GREATER_EQUAL | 7
//! SGD_DEPTH_FUNC_ALWAYS        | 8
SGD_API void SGD_DECL sgd_SetMaterialDepthFunc(SGD_Material material, int depthFunc);

//! Set material primitive culling mode.
//!
//! `cullMode` should be one of the following:
//!
//! Cull mode           | Integer value | Description
//! --------------------| --------------|------------
//! SGD_CULL_MODE_NONE  | 1             | Don't cull any primitives.
//! SGD_CULL_MODE_FRONT | 2             | Cull front facing primitives.
//! SGD_CULL_MODE_BACK  | 3             | Cull back facing primitives.
SGD_API void SGD_DECL sgd_SetMaterialCullMode(SGD_Material material, int cullMode);

//! Set material vector4 property
SGD_API void SGD_DECL sgd_SetMaterialVector4f(SGD_Material material, SGD_String property, float x, float y, float z, float a);

//! Set material vector3 property
SGD_API void SGD_DECL sgd_SetMaterialVector3f(SGD_Material material, SGD_String property, float x, float y, float z);

//! Set material vector2 property
SGD_API void SGD_DECL sgd_SetMaterialVector2f(SGD_Material material, SGD_String property, float x, float y);

//! Set material float property
SGD_API void SGD_DECL sgd_SetMaterialFloat(SGD_Material material, SGD_String property, float n);

//! Set material texture property
SGD_API void SGD_DECL sgd_SetMaterialTexture(SGD_Material material, SGD_String property, SGD_Texture texture);

//! Set a material

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

//! Load a skybox.
SGD_API SGD_Skybox SGD_DECL sgd_LoadSkybox(SGD_String path, float roughness);

//! Create a new skybox.
SGD_API SGD_Skybox SGD_DECL sgd_CreateSkybox();

//! Set skybox texture.
SGD_API void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox skybox, SGD_Texture texture);

//! Set skybox roughness in the range 0 to 1.
SGD_API void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox skybox, float roughness);

// ***** Model *****

//! @cond Animation ode constants used by sgd_AnimateModel
#define SGD_ANIMATION_MODE_ONE_SHOT 1
#define SGD_ANIMATION_MODE_LOOP 2
#define SGD_ANIMATION_MODE_PING_PONG 3
//! @endcond

//! Load a model.
SGD_API SGD_Model SGD_DECL sgd_LoadModel(SGD_String path);

//! Load a boned model.
SGD_API SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path, SGD_Bool skinned);

//! Create a new model.
SGD_API SGD_Model SGD_DECL sgd_CreateModel();

//! Set model mesh.
SGD_API void SGD_DECL sgd_SetModelMesh(SGD_Model model, SGD_Mesh mesh);

//! Set model color.
SGD_API void SGD_DECL sgd_SetModelColor(SGD_Model model, float red, float green, float blue, float alpha);

//! Animate a model.
//!
//! @mode should be one of the following:
//!
//! Animation mode              | Integer value | meaning
//! ----------------------------|---------------|--------
//! SGD_ANIMATION_MODE_ONE_SHOT | 1             | Animation plays forward once then ends.
//! SGD_ANIMATION_MODE_LOOP     | 2             | Animation plays forward repeatedly.
//! SGD_ANIMATION_PING_PING     | 3             | Animation plays forward then backwards repeatedly.
SGD_API void SGD_DECL sgd_AnimateModel(SGD_Model model, int animation, float time, int mode);

// ***** Sprite *****

//! Create a new sprite.
SGD_API SGD_Sprite SGD_DECL sgd_CreateSprite();

//! Set sprite material.
SGD_API void SGD_DECL sgd_SetSpriteMaterial(SGD_Sprite sprite, SGD_Material material);

//! Set sprite color.
SGD_API void SGD_DECL sgd_SetSpriteColor(SGD_Sprite sprite, float red, float green, float blue, float alpha);

//! Set sprite rect, defaults to -.5, -.5, .5, .5.
SGD_API void SGD_DECL sgd_SetSpriteRect(SGD_Sprite sprite, float minX, float minY, float maxX, float maxY);

// ***** Camera *****

//! Create a perspective camera.
SGD_API SGD_Camera SGD_DECL sgd_CreatePerspectiveCamera();

//! Create an orthographic camera.
SGD_API SGD_Camera SGD_DECL sgd_CreateOrthographicCamera();

//! Set camera vertical field of view in degrees.
SGD_API void SGD_DECL sgd_SetCameraFOV(SGD_Camera camera, float fovY);

//! Set camera near clipping plane.
SGD_API void SGD_DECL sgd_SetCameraNear(SGD_Camera camera, float near);

//! Set camera far clipping plane.
SGD_API void SGD_DECL sgd_SetCameraFar(SGD_Camera camera, float far);

// ***** Light *****

//! Create a directional light.
SGD_API SGD_Light SGD_DECL sgd_CreateDirectionalLight();

//! Create a point light.
SGD_API SGD_Light SGD_DECL sgd_CreatePointLight();

//! Create a spot light.
SGD_API SGD_Light SGD_DECL sgd_CreateSpotLight();

//! Set a light's color.
SGD_API void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);

//! Set a point/spot light's range.
SGD_API void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);

//! Set a point/pot light's falloff.
SGD_API void SGD_DECL sgd_SetLightFalloff(SGD_Light light, float falloff);

//! Set a spot light's inner cone angle in degrees.
SGD_API void SGD_DECL sgd_SetLightInnerConeAngle(SGD_Light light, float angle);

//! Set a spot light's outer cone angle in degrees.
SGD_API void SGD_DECL sgd_SetLightOuterConeAngle(SGD_Light light, float angle);

// ***** Entity *****

//! Copy an entity and its children recursively.
SGD_API SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity entity);

//! Set an entity's parent.
SGD_API void SGD_DECL sgd_SetEntityParent(SGD_Entity entity, SGD_Entity parent);

//! Get an entity's parent.
SGD_API SGD_Entity SGD_DECL sgd_EntityParent(SGD_Entity entity);

//! Set an  entity's position in world space.
SGD_API void SGD_DECL sgd_SetEntityPosition(SGD_Entity entity, float tx, float ty, float tz);

//! Set an  entity's rotation in world space.
SGD_API void SGD_DECL sgd_SetEntityRotation(SGD_Entity entity, float rx, float ry, float rz);

//! Translate an entity in world space.
SGD_API void SGD_DECL sgd_TranslateEntity(SGD_Entity entity, float tx, float ty, float tz);

//! Rotate an entity in world space.
SGD_API void SGD_DECL sgd_RotateEntity(SGD_Entity entity, float rx, float ry, float rz);

//! Move an entity relative to it's current orientation.
SGD_API void SGD_DECL sgd_MoveEntity(SGD_Entity entity, float tx, float ty, float tz);

//! Turn an entity relative to it's current orientation.
SGD_API void SGD_DECL sgd_TurnEntity(SGD_Entity entity, float rx, float ry, float rz);

//! Get the X component of an entity's position in world space.
SGD_API float SGD_DECL sgd_EntityX(SGD_Entity entity);

//! Get the Y component of an entity's position in world space.
SGD_API float SGD_DECL sgd_EntityY(SGD_Entity entity);

//! Get the Z component of an entity's position in world space.
SGD_API float SGD_DECL sgd_EntityZ(SGD_Entity entity);

//! Get the X rotation component (ie: 'pitch') of an entity's orientation in world space.
SGD_API float SGD_DECL sgd_EntityRX(SGD_Entity entity);

//! Get the Y rotation component (ie: 'yaw') of an entity's orientation in world space.
SGD_API float SGD_DECL sgd_EntityRY(SGD_Entity entity);

//! Get the Z rotation component (ie: 'roll') of an entity's orientation in world space.
SGD_API float SGD_DECL sgd_EntityRZ(SGD_Entity entity);

#endif
