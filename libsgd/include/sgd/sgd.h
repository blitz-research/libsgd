#ifndef SGD_SGD_H_INCLUDED
#define SGD_SGD_H_INCLUDED

#include "keycodes.h"

#include <stdint.h>

//! @file

//! @mainpage
//!
//! Welcome to the LibSGD reference documentation.
//!
//! Quick links: sgd.h keycodes.h

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

#if _MSC_VER && !_WIN64 // 32 bit blitz3d support needs __stdcall
#define SGD_DECL __stdcall
#elif _MSC_VER
#define SGD_DECL __cdecl
#else
#define SGD_DECL
#endif

#define SGD_TRUE 1
#define SGD_FALSE 0

#define SGD_PI 3.14159265359f
#define SGD_TWO_PI 6.28318530718f
#define SGD_HALF_PI 1.5707963268f
#define SGD_DEGREES_TO_RADIANS .01745329252f
#define SGD_RADIANS_TO_DEGREES 57.295779513f

//! @endcond

//! @name Typedefs
//! @{

typedef int SGD_Bool;
typedef int SGD_Handle;
typedef const char* SGD_String;

typedef SGD_Handle SGD_Font;
typedef SGD_Handle SGD_Sound;
typedef SGD_Handle SGD_Texture;
typedef SGD_Handle SGD_Material;
typedef SGD_Handle SGD_Image;
typedef SGD_Handle SGD_Surface;
typedef SGD_Handle SGD_Mesh;
typedef SGD_Handle SGD_Entity;
typedef SGD_Handle SGD_Camera;
typedef SGD_Handle SGD_Light;
typedef SGD_Handle SGD_Model;
typedef SGD_Handle SGD_Sprite;
typedef SGD_Handle SGD_Skybox;
typedef SGD_Handle SGD_Collider;
typedef SGD_Handle SGD_RenderEffect;

#if UINTPTR_MAX == 0xffffffffffffffff
typedef double SGD_Real;
#else
typedef float SGD_Real;
#endif

//! @}

//! @name System
//! @{

//! Start up libsgd.
SGD_API void SGD_DECL sgd_Init();

//! Shut down libsgd.
SGD_API void SGD_DECL sgd_Terminate();

//! Set global configuration variable.
//!
//! Config var name           | Default | Description
//!---------------------------|---------|------------
//! log.logfilePath           |         | Logfile path, defaults to "~/.sgd/log.txt".
//! log.logfileEnabled        | "1"     | Enables logging to logfile.
//! log.stdoutEnabled         | "1"     | Enables logging to stdout.
//! gltf.loggingEnabled       | "0"     | Enables logging by the gltf loader.
//! dawn.backendType          |         | Backend for Dawn to use, one of: "D3D12", "D3D11", "Vulkan", "Metal". Defaults to "D3D12" on 64 bit Windows, "D3D11" on 32 bit Windows, "Vulkan" on 64 bit Linux and "Metal" on 64 bit MacOS.
//! render.vsyncEnabled       | "1"     | Hack that fixes https://issues.chromium.org/issues/42241496#comment3
//! render.shadowPassEnabled  | "1"     | Enables shadow rendering.
//! render.opaquePassEnabled  | "1"     | Enables opaque rendering.
//! render.blendPassEnabled   | "1"     | Enables blended rendering.
//! render.effectPassEnabled  | "1"     | Enables effects rendering.
SGD_API void SGD_DECL sgd_SetConfigVar(SGD_String name, SGD_String value);

//! Set global configuration variable. The returned value is valid until the next call to sgd_GetConfigVar.
//SGD_API SGD_String SGD_DECL sgd_GetConfigVar(SGD_String name);

//! Set error handler callback.
SGD_API void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL* handler)(SGD_String error, void* context), void* context);

//! Generate runtime error.
SGD_API void SGD_DECL sgd_Error(SGD_String error);

//! Generate modal alert dialog.
SGD_API void SGD_DECL sgd_Alert(SGD_String message);

//! Write line of text to log.
SGD_API void SGD_DECL sgd_Log(SGD_String line);

//! Return width of desktop in pixels.
SGD_API int SGD_DECL sgd_GetDesktopWidth();

//! Return height of desktop in pixels.
SGD_API int SGD_DECL sgd_GetDesktopHeight();

//! @cond Event mask  constants returned by sgd_NextEvent.
#define SGD_EVENT_MASK_CLOSE_CLICKED 1
#define SGD_EVENT_MASK_SIZE_CHANGED  2
#define SGD_EVENT_MASK_LOST_FOCUS    4
#define SGD_EVENT_MASK_GOT_FOCUS     8
#define SGD_EVENT_MASK_SUSPENDED     16
#define SGD_EVENT_MASK_RESUMED       32
//! @endcond

//! Poll system for events, returns bit mask of events types that occured.
//!
//! The returned bit mask can contain any of the following values:
//!
//! Event type mask                | Integer value | Description
//! -------------------------------|---------------|------------
//! SGD_EVENT_MASK_CLOSE_CLICKED   | 1             | Window close button clicked
//! SGD_EVENT_MASK_SIZE_CHANGED    | 2             | Window size changed
//! SGD_EVENT_MASK_LOST_FOCUS      | 4             | Window lost input focus
//! SGD_EVENT_MASK_GOT_FOCUS       | 8             | Window received input focus
//! SGD_EVENT_MASK_SUSPENDED       | 16            | App suspended
//! SGD_EVENT_MASK_RESUMED         | 32            | App resumed
SGD_API int SGD_DECL sgd_PollEvents();

//! @cond Debug memory state, does nothing in release builds.
SGD_API void SGD_DECL sgd_DebugMemory();
//! @endcond

//! @}

//! @name Window
//! @{

//! @cond Window states for use with SetWindowState
#define SGD_WINDOW_STATE_CLOSED     0
#define SGD_WINDOW_STATE_MINIMIZED  1
#define SGD_WINDOW_STATE_NORMAL     2
#define SGD_WINDOW_STATE_MAXIMIZED  3
#define SGD_WINDOW_STATE_FULLSCREEN 4
//! @endcond

//! @cond Window flags for use with CreateWindow.
#define SGD_WINDOW_FLAGS_FULLSCREEN      1
#define SGD_WINDOW_FLAGS_RESIZABLE       2
#define SGD_WINDOW_FLAGS_CENTERED        4
#define SGD_WINDOW_FLAGS_FULLSCREEN_60HZ 8
//! @endcond

//! Create a new window.
//!
//! `flags` should be one or more of the following bit mask values:
//!
//! Window flag                      | Integer value | Description
//! ---------------------------------|---------------|------------
//! SGD_WINDOW_FLAGS_FULLSCREEN      | 1             | Create a fullscreen window
//! SGD_WINDOW_FLAGS_RESIZABLE       | 2             | Create a resizable window
//! SGD_WINDOW_FLAGS_CENTERED        | 4             | Open window centered on desktop
//! SGD_WINDOW_FLAGS_FULLSCREEN_60HZ | 8             | Create a fullscreen 60 hz window
SGD_API void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags);

//! Destroy window.
SGD_API void SGD_DECL sgd_DestroyWindow();

//! Set window position.
SGD_API void SGD_DECL sgd_SetWindowPosition(int x, int y);

//! Get window x.
SGD_API int SGD_DECL sgd_GetWindowX();

//! Get window y.
SGD_API int SGD_DECL sgd_GetWindowY();

//! Set window size.
SGD_API void SGD_DECL sgd_SetWindowSize(int width, int height);

//! Get window width.
SGD_API int SGD_DECL sgd_GetWindowWidth();

//! Get window height.
SGD_API int SGD_DECL sgd_GetWindowHeight();

//! Set window title.
SGD_API void SGD_DECL sgd_SetWindowTitle(SGD_String title);

//! Get window title. The returned string will be valid until the next call to sgd_GetWindowTitle().
SGD_API SGD_String SGD_DECL sgd_GetWindowTitle();

//! Set fullscreen mode.
SGD_API void SGD_DECL sgd_SetFullscreenMode(int width, int height, int hertz);


//! Set window state.
//!
//! `state` should be one of the following values:
//!
//! State                          | Integer value | Description
//! -------------------------------|---------------|------------
//! SGD_WINDOW_STATE_CLOSED        | 0             | Ignored - returned by sgd_GetWindowState only
//! SGD_WINDOW_STATE_MINIMIZED     | 1             | Minimize window
//! SGD_WINDOW_STATE_NORMAL        | 2			   | Restore minimized or maximized window, or exit fullscreen mode
//! SGD_WINDOW_STATE_MAXIMIZED     | 3             | Maximize window
//! SGD_WINDOW_STATE_FULLSCREEN    | 4             | Re-enter fullscreen mode
SGD_API void SGD_DECL sgd_SetWindowState(int state);

//! Get window state. See sgd_SetWindowState.
SGD_API int SGD_DECL sgd_GetWindowState();

//! @}

//! @name User Input
//! @{

//! True if key is currently held down.
SGD_API SGD_Bool SGD_DECL sgd_IsKeyDown(int keyCode);

//! True if key was pressed down last frame.
SGD_API SGD_Bool SGD_DECL sgd_IsKeyHit(int keyCode);

//! Get next unicode character from keyboard input queue.
SGD_API int SGD_DECL sgd_GetChar();

//! Clear keyboard input queue.
SGD_API void SGD_DECL sgd_FlushChars();

//! Mouse X position in window coordinates.
SGD_API float SGD_DECL sgd_GetMouseX();

//! Mouse Y position in window coordinates.
SGD_API float SGD_DECL sgd_GetMouseY();

//! Mouse Z (scrollwheel) position.
SGD_API float SGD_DECL sgd_GetMouseZ();

//! Mouse X velocity.
SGD_API float SGD_DECL sgd_GetMouseVX();

//! Mouse Y velocity.
SGD_API float SGD_DECL sgd_GetMouseVY();

//! Mouse Z (scrollwheel) velocity.
SGD_API float SGD_DECL sgd_GetMouseVZ();

//! Set Mouse Z (scrollwheel) position.
SGD_API void SGD_DECL sgd_SetMouseZ(float z);

//! @cond Cursor modes for use with SetMouseCursorMode.
#define SGD_MOUSE_CURSOR_MODE_NORMAL 1
#define SGD_MOUSE_CURSOR_MODE_HIDDEN 2
#define SGD_MOUSE_CURSOR_MODE_DISABLED 3
#define SGD_MOUSE_CURSOR_MODE_CAPTURED 4
//! @endcond

//! Set mouse cursor mode.
//!
//! `cursorMode` should be one or more of the following bit mask values:
//!
//! Cursor mode                    | Integer value | Description
//! -------------------------------|---------------|------------
//! SGD_MOUSE_CURSOR_MODE_NORMAL   | 1             | Normal mouse cursor behaviour
//! SGD_MOUSE_CURSOR_MODE_HIDDEN   | 2             | Mouse cursor is hidden
//! SGD_MOUSE_CURSOR_MODE_DISABLED | 3             | Mouse cursor is hidden and locked to window
//! SGD_MOUSE_CURSOR_MODE_CAPTURED | 4             | Mouse cursor is locked to window
SGD_API void SGD_DECL sgd_SetMouseCursorMode(int cursorMode);

//! True if mouse button is curently held down.
SGD_API SGD_Bool SGD_DECL sgd_IsMouseButtonDown(int button);

//! True if mouse button was pressed down last frame.
SGD_API SGD_Bool SGD_DECL sgd_IsMouseButtonHit(int button);

//! True if gamepad is currently connected.
SGD_API SGD_Bool SGD_DECL sgd_IsGamepadConnected(int gamepad);

//! True if gamepad button currently held down.
SGD_API SGD_Bool SGD_DECL sgd_IsGamepadButtonDown(int gamepad, int button);

//! True if gamepad button was pressed down last frame.
SGD_API SGD_Bool SGD_DECL sgd_IsGamepadButtonHit(int gamepad, int button);

//! Value in the range -1 to 1 representing joystick axis position.
SGD_API float SGD_DECL sgd_GetGamepadAxis(int gamepad, int axis);

//! @}

//! @name Texture
//! @{

//! @cond Texture format constants
#define SGD_TEXTURE_FORMAT_R8 1
#define SGD_TEXTURE_FORMAT_RG8 2
#define SGD_TEXTURE_FORMAT_RGBA8 3
#define SGD_TEXTURE_FORMAT_SRGBA8 4
#define SGD_TEXTURE_FORMAT_RGBA16F 5
#define SGD_TEXTURE_FORMAT_DEPTH32F 6
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
//
#define SGD_TEXTURE_FLAGS_ENVMAP_DEFAULT   0x38
#define SGD_TEXTURE_FLAGS_MATERIAL_DEFAULT 0x18
#define SGD_TEXTURE_FLAGS_IMAGE_DEFAULT    0x1f
//! @endcond

//! Load a new texture.
//!
//! @param `path` is the file path of the texture to load.
//!
//! @param `format` should be one of the following:
//!
//! Texture format              | Integer value | Bytes per texel | Description
//! ----------------------------|---------------|-----------------|------------
//! SGD_TEXTURE_FORMAT_R8       | 1             | 1               | Unsigned byte per component linear red.
//! SGD_TEXTURE_FORMAT_RG8      | 2             | 2               | Unsigned byte per component linear red/green.
//! SGD_TEXTURE_FORMAT_RGBA8    | 3             | 4               | Unsigned byte per component linear red/green/blue/alpha
//! SGD_TEXTURE_FORMAT_SRGBA8   | 4             | 4               | Unsigned byte per component standard red/green/blue/alpha
//! SGD_TEXTURE_FORMAT_RGBA16F  | 5             | 8               | 16 bit float per component linear red/green/blue/alpha
//! SGD_TEXTURE_FORMAT_DEPTH32F | 6             | 4               | 32 bit float per component depth
//!
//! Note that you should generally use the 'standard' formats when dealing with WYSIWG images,
//! 'linear' when dealing with algorithmically generated data.
//!
//! @param `flags` should be one or more of the following bitmasks:
//!
//! Texture flags               | Integer value | Description
//! ----------------------------|---------------|------------
//! SGD_TEXTURE_FLAGS_NONE      | 0x00          | No texture flags
//! SGD_TEXTURE_FLAGS_CLAMP_U   | 0x01          | Clamp texture U coordinate
//! SGD_TEXTURE_FLAGS_CLAMP_V   | 0x02          | Clamp texture V coordinate
//! SGD_TEXTURE_FLAGS_CLAMP_W   | 0x04          | Clamp texture W coordinate
//! SGD_TEXTURE_FLAGS_CLAMP     | 0x07          | Clamp texture U, V, W coordinates
//! SGD_TEXTURE_FLAGS_FILTER    | 0x08          | Filter magnified texels
//! SGD_TEXTURE_FLAGS_MIPMAP    | 0x10          | Mipmap minified texels
//! SGD_TEXTURE_FLAGS_CUBE      | 0x20          | Create a cube texture
//!
//! Some convenient default combinations of texture flags are also defined:
//!
//! SGD_TEXTURE_FLAGS_ENVMAP_DEFAULT   0x38
//! SGD_TEXTURE_FLAGS_MATERIAL_DEFAULT 0x18
//! SGD_TEXTURE_FLAGS_IMAGE_DEFAULT    0x1f
SGD_API SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags);

//! @}

//! @name Material
//! @{

//! @cond blend mode constants
#define SGD_BLEND_MODE_OPAQUE      1
#define SGD_BLEND_MODE_ALPHA_MASK  2
#define SGD_BLEND_MODE_ALPHA_BLEND 3
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

//! Create a new prelit matterial.
SGD_API SGD_Material SGD_DECL sgd_CreatePrelitMaterial();

//! Load a new prelit material.
SGD_API SGD_Material SGD_DECL sgd_LoadPrelitMaterial(SGD_String path);

//! Set material blend mode.
//!
//! `blendMode` should be one of the following:
//!
//! Blend mode                 | Integer value | Description
//! ---------------------------|---------------|------------
//! SGD_BLEND_MODE_OPAQUE      | 1             | Opaque
//! SGD_BLEND_MODE_ALPHA_MASK  | 2             | Alpha mask - fragments with alpha >= 0.5 are rendered, others are discarded
//! SGD_BLEND_MODE_ALPHA_BLEND | 3             | Alpha blend - fragments are blended ing using premultipled alpha
SGD_API void SGD_DECL sgd_SetMaterialBlendMode(SGD_Material material, int blendMode);

//! Set material depth comparison function.
//!
//! `depthFunc` should be one of the following:
//!
//! Depth function               | Integer value
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
//! SGD_CULL_MODE_NONE  | 1             | Don't cull any primitives
//! SGD_CULL_MODE_FRONT | 2             | Cull front facing primitives
//! SGD_CULL_MODE_BACK  | 3             | Cull back facing primitives
SGD_API void SGD_DECL sgd_SetMaterialCullMode(SGD_Material material, int cullMode);

//! Set material vector4 property.
SGD_API void SGD_DECL sgd_SetMaterialVector4f(SGD_Material material, SGD_String property, float x, float y, float z, float w);

//! Set material vector3 property.
SGD_API void SGD_DECL sgd_SetMaterialVector3f(SGD_Material material, SGD_String property, float x, float y, float z);

//! Set material vector2 property.
SGD_API void SGD_DECL sgd_SetMaterialVector2f(SGD_Material material, SGD_String property, float x, float y);

//! Set material float property.
SGD_API void SGD_DECL sgd_SetMaterialFloat(SGD_Material material, SGD_String property, float n);

//! Set material texture property.
SGD_API void SGD_DECL sgd_SetMaterialTexture(SGD_Material material, SGD_String property, SGD_Texture texture);

//! @}

//! @name Mesh
//! @{

//! Load a new mesh.
SGD_API SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path);

//! Create a new box mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
											SGD_Material material);

//! Create a new sphere mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material);

//! Create a new cylinder mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateCylinderMesh(float height, float radius, int segs, SGD_Material material);

//! Create a new cone mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateConeMesh(float height, float radius, int segs, SGD_Material material);

//! Create a new torus mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs, SGD_Material material);

//! Set mesh shadow casting enabled, defaults to true.
SGD_API void SGD_DECL sgd_SetMeshShadowsEnabled(SGD_Mesh mesh, SGD_Bool enabled);

//! Get mesh shadow casting enabled.
SGD_API SGD_Bool SGD_DECL sgd_IsMeshShadowsEnabled(SGD_Mesh mesh);

//! Copy mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CopyMesh(SGD_Mesh mesh);

//! Update mesh normals.
SGD_API void SGD_DECL sgd_UpdateMeshNormals(SGD_Mesh mesh);

//! Update mesh tangets. Mesh must have SGD_MESH_TANGENTS_ENABLED flag.
SGD_API void SGD_DECL sgd_UpdateMeshTangents(SGD_Mesh mesh);

//! Fit mesh to a bounding box.
SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh mesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
								  SGD_Bool uniform);

//! Transform mesh by translation, rotation, scale.
SGD_API void SGD_DECL sgd_TFormMesh(SGD_Mesh mesh, float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz);

//! Transform mesh texture coordinates.
SGD_API void SGD_DECL sgd_TFormMeshTexCoords(SGD_Mesh mesh, float scaleX, float scaleY, float offsetX, float offsetY);

//! Flip mesh.
SGD_API void SGD_DECL sgd_FlipMesh(SGD_Mesh mesh);

//! @}

//! @name Mesh Building
//! @{

//! Create a new custom mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateMesh(int vertexCount, int flags);

//! Add uninitialized vertices to a mesh, returning index of the first new vertex.
SGD_API void SGD_DECL sgd_ResizeVertices(SGD_Mesh mesh, int count);

//! Get number of vertices in mesh.
SGD_API int SGD_DECL sgd_GetVertexCount(SGD_Mesh mesh);

//! Add a vertex to a mesh, returning index of new vertex.
SGD_API int SGD_DECL sgd_AddVertex(SGD_Mesh mesh, float x, float y, float z, float nx, float ny, float nz, float s, float t);

//! Set vertex.
SGD_API void SGD_DECL sgd_SetVertex(SGD_Mesh mesh, int vertex, float x, float y, float z, float nx, float ny, float nz, float s, float t);

//! Set vertex position.
SGD_API void SGD_DECL sgd_SetVertexPosition(SGD_Mesh mesh, int vertex, float x, float y, float z);

//! Set vertex normal.
SGD_API void SGD_DECL sgd_SetVertexNormal(SGD_Mesh mesh, int vertex, float nx, float ny, float nz);

//! Set vertex tangent.
SGD_API void SGD_DECL sgd_SetVertexTangent(SGD_Mesh mesh, int vertex, float tx, float ty, float tz, float tw);

//! Set vertex color.
SGD_API void SGD_DECL sgd_SetVertexColor(SGD_Mesh mesh, int vertex, float r, float g, float b, float a);

//! Set vertex texture coordinate 0.
SGD_API void SGD_DECL sgd_SetVertexTexCoord0(SGD_Mesh mesh, int vertex, float u0, float v0);

//! Get vertex position x coordinate.
SGD_API float SGD_DECL sgd_GetVertexX(SGD_Mesh mesh, int vertex);

//! Get vertex position y coordinate.
SGD_API float SGD_DECL sgd_GetVertexY(SGD_Mesh mesh, int vertex);

//! Get vertex position z coordinate.
SGD_API float SGD_DECL sgd_GetVertexZ(SGD_Mesh mesh, int vertex);

//! Get vertex normal x component.
SGD_API float SGD_DECL sgd_GetVertexNX(SGD_Mesh mesh, int vertex);

//! Get vertex normal y component.
SGD_API float SGD_DECL sgd_GetVertexNY(SGD_Mesh mesh, int vertex);

//! Get vertex normal z component.
SGD_API float SGD_DECL sgd_GetVertexNZ(SGD_Mesh mesh, int vertex);

//! Get vertex tangent x component.
SGD_API float SGD_DECL sgd_GetVertexTX(SGD_Mesh mesh, int vertex);

//! Get vertex tangent y component.
SGD_API float SGD_DECL sgd_GetVertexTY(SGD_Mesh mesh, int vertex);

//! Get vertex tangent z component.
SGD_API float SGD_DECL sgd_GetVertexTZ(SGD_Mesh mesh, int vertex);

//! Get vertex tangent w component.
SGD_API float SGD_DECL sgd_GetVertexTW(SGD_Mesh mesh, int vertex);

//! Get vertex color red component.
SGD_API float SGD_DECL sgd_GetVertexRed(SGD_Mesh mesh, int vertex);

//! Get vertex color green component.
SGD_API float SGD_DECL sgd_GetVertexGreen(SGD_Mesh mesh, int vertex);

//! Get vertex color blue component.
SGD_API float SGD_DECL sgd_GetVertexBlue(SGD_Mesh mesh, int vertex);

//! Get vertex color alpha component.
SGD_API float SGD_DECL sgd_GetVertexAlpha(SGD_Mesh mesh, int vertex);

//! Get vertex texture coordinate 0 u component.
SGD_API float SGD_DECL sgd_GetVertexU0(SGD_Mesh mesh, int vertex);

//! Get vertex tangent coorindate 1 y component.
SGD_API float SGD_DECL sgd_GetVertexV0(SGD_Mesh mesh, int vertex);

// ***** Surfaces *****

//! Create a new Surface and add it to mesh.
SGD_API SGD_Surface SGD_DECL sgd_CreateSurface(SGD_Mesh mesh, SGD_Material material, int triangleCount);

//! Get mesh surface count.
SGD_API int SGD_DECL sgd_GetSurfaceCount(SGD_Mesh mesh);

//! Get mesh surface.
SGD_API SGD_Surface SGD_DECL sgd_GetSurface(SGD_Mesh mesh, int surface);

//! Get surface material.
SGD_API SGD_Material SGD_DECL sgd_GetSurfaceMaterial(SGD_Surface surface);

//! Add empty triangles to surface, returning index of first new triangle.
SGD_API void SGD_DECL sgd_ResizeTriangles(SGD_Surface surface, int count);

//! Get number of triangles in surface.
SGD_API int SGD_DECL sgd_GetTriangleCount(SGD_Surface surface);

//! Add triangle to surface, returning index of new triangle.
SGD_API int SGD_DECL sgd_AddTriangle(SGD_Surface surface, int v0, int v1, int v2);

//! Update existing triangle vertices in surface.
SGD_API void SGD_DECL sgd_SetTriangle(SGD_Surface surface, int triangle, int v0, int v1, int v2);

//! Get mesh vertex index of a triangle corner. Vertex must be 0, 1 or 2.
SGD_API int SGD_DECL sgd_GetTriangleVertex(SGD_Surface surface, int triangle, int vertex);


//! @}

//! @name Font
//! @{

//! Load a new font
SGD_API SGD_Font SGD_DECL sgd_LoadFont(SGD_String path, float height);

//! Get width of text.
SGD_API float SGD_DECL sgd_GetTextWidth(SGD_Font font, SGD_String text);

//! get height of font.
SGD_API float SGD_DECL sgd_GetFontHeight(SGD_Font font);

//! @}

//! @name Image
//! @{

//! Load an image for use with 3D sprites or Draw2DImage.
SGD_API SGD_Image SGD_DECL sgd_LoadImage(SGD_String path, int depth);

//! @cond
#define SGD_IMAGE_VIEW_MODE_FIXED 1
#define SGD_IMAGE_VIEW_MODE_FREE 2
#define SGD_IMAGE_VIEW_MODE_UPRIGHT 3
//! @endcond

//! Set view mode for use with 3D sprites.
//!
//! `imageViewMode` should be one of the following values:
//!
//! Image view mode             | Integer value | Description
//!-----------------------------|---------------|------------
//! SGD_IMAGE_VIEW_MODE_FIXED   | 1             | Image is rotated to face camera when rendered.
//! SGD_IMAGE_VIEW_MODE_FREE    | 2             | Image is rendered without taking camera position into account.
//! SGD_IMAGE_VIEW_MODE_UPRIGHT | 3             | Image is rotated around local Y axis to face camera, useful for tree like sprites.
SGD_API void SGD_DECL sgd_SetImageViewMode(SGD_Image image, int viewMode);

//! Set rect for use with 3D sprites.
SGD_API void SGD_DECL sgd_SetImageRect(SGD_Image image, float minX, float minY, float maxX, float maxY);

//! Set 2D handle for use with Draw2DImage.
SGD_API void SGD_DECL sgd_SetImage2DHandle(SGD_Image image, float x, float y);

//! Set image blend mode.
SGD_API void SGD_DECL sgd_SetImageBlendMode(SGD_Image image, int blendMode);

//! Get width of image.
SGD_API int SGD_DECL sgd_GetImageWidth(SGD_Image image);

//! Get height of image.
SGD_API int SGD_DECL sgd_GetImageHeight(SGD_Image image);

//! Get depth (animation frames) of image.
SGD_API int SGD_DECL sgd_GetImageDepth(SGD_Image image);

//! @}

//! @name 2D Overlay
//! @{

//! Set current fill color for drawing shapes.
SGD_API void SGD_DECL sgd_Set2DFillColor(float red, float green, float blue, float alpha);

//! Set current fill material for drawing shapes.
SGD_API void SGD_DECL sgd_Set2DFillMaterial(SGD_Material material);

//! SetCurrent file enabled flag for drawing shapes.
SGD_API void SGD_DECL sgd_Set2DFillEnabled(SGD_Bool enabled);

//! Set current outline color for drawing shapes.
SGD_API void SGD_DECL sgd_Set2DOutlineColor(float red, float green, float blue, float alpha);

//! Set current outline width, defaults to 3.
SGD_API void SGD_DECL sgd_Set2DOutlineWidth(float width);

//! SetCurrent outline enabled flag for drawing shapes.
SGD_API void SGD_DECL sgd_Set2DOutlineEnabled(SGD_Bool enabled);

//! Set current line width, defaults to 3.
SGD_API void SGD_DECL sgd_Set2DLineWidth(float width);

//! Set current point size, defaults to 3.
SGD_API void SGD_DECL sgd_Set2DPointSize(float size);

//! Set current font.
SGD_API void SGD_DECL sgd_Set2DFont(SGD_Font font);

//! Set current text color.
SGD_API void SGD_DECL sgd_Set2DTextColor(float red, float green, float blue, float alpha);

//! Get width of 2d text in the current 2D font.
SGD_API float SGD_DECL sgd_Get2DTextWidth(SGD_String text);

//! Get height of current 2d font.
SGD_API float SGD_DECL sgd_Get2DFontHeight();

//! Clear the current 2d overlay.
SGD_API void SGD_DECL sgd_Clear2D();

//! Push current draw list to layer stack, effectively 'freezing' it as a background layer until popped.
SGD_API void SGD_DECL sgd_Push2DLayer();

//! Pop layer stack.
SGD_API void SGD_DECL sgd_Pop2DLayer();

//! Draw point using current fill and outline colors.
SGD_API void SGD_DECL sgd_Draw2DPoint(float x0, float y0);

//! Draw line using current fill and outline colors.
SGD_API void SGD_DECL sgd_Draw2DLine(float x0, float y0, float x1, float y1);

//! Draw rect using current fill and outline colors.
SGD_API void SGD_DECL sgd_Draw2DRect(float minX, float minY, float maxX, float maxY);

//! Draw image using current fill color.
SGD_API void SGD_DECL sgd_Draw2DImage(SGD_Image image, float x, float y, float frame);

//! Draw oval using current fill and outline colors.
SGD_API void SGD_DECL sgd_Draw2DOval(float minX, float minY, float maxX, float maxY);

//! Draw text using current text color.
SGD_API void SGD_DECL sgd_Draw2DText(SGD_String text, float x,float y);

//! @}

//! @name Audio
//! @{

//! Load a new sound.
SGD_API SGD_Sound SGD_DECL sgd_LoadSound(SGD_String path);

//! Play a sound, returning an audio stream..
SGD_API int SGD_DECL sgd_PlaySound(SGD_Sound sound);

//! Cue a sound. The returned audio stream must be unpaused before it will start.
SGD_API int SGD_DECL sgd_CueSound(SGD_Sound sound);

//! Set audio volume.
SGD_API void SGD_DECL sgd_SetAudioVolume(int audio, float volume);

//! Set audio pan.
SGD_API void SGD_DECL sgd_SetAudioPan(int audio, float pan);

//! Set audio pitch scale.
SGD_API void SGD_DECL sgd_SetAudioPitchScale(int audio, float scale);

//! Set audio looping flag.
SGD_API void SGD_DECL sgd_SetAudioLooping(int audio, SGD_Bool looping);

//! Set audio paused flag.
SGD_API void SGD_DECL sgd_SetAudioPaused(int audio, SGD_Bool paused);

//! Get audio valid flag. Audio is valid if it is playing or paused.
SGD_API SGD_Bool SGD_DECL sgd_IsAudioValid(int audio);

//! Stop audio.
SGD_API void SGD_DECL sgd_StopAudio(int audio);

//! @}

//! @name Scene
//! @{

//! Release SGD resources currently in use.
SGD_API void SGD_DECL sgd_ClearScene();

//! Set scene ambient light color.
SGD_API void SGD_DECL sgd_SetAmbientLightColor(float red, float green, float blue, float alpha);

//! Set scene clear color.
SGD_API void SGD_DECL sgd_SetClearColor(float red, float green, float blue, float alpha);

//! Set scene clear depth.
SGD_API void SGD_DECL sgd_SetClearDepth(float depth);

//! Set scene environment texture.
SGD_API void SGD_DECL sgd_SetEnvTexture(SGD_Texture texture);

//! Set cascading shadow map texture size. Defaults to 2048, must be a power of 2.
SGD_API void SGD_DECL sgd_SetCSMTextureSize(int textureSize);

//! Set max shadow casting directional lights. Defaults to 4, must be <= max directional lights (4).
SGD_API void SGD_DECL sgd_SetMaxCSMLights(int maxLights);

//! Set split distances for CSM shadow, last value should match camera far. Defaults to 16, 64, 256, 1024.
SGD_API void SGD_DECL sgd_SetCSMSplitDistances(float sklit0, float split1, float split2, float split3);

//! Set max range of potential CSM obscurers. Defaults to 1000.
SGD_API void SGD_DECL sgd_SetCSMClipRange(float range);

//! Set depth bia for CSM shadows, increase to reduce 'shadow acne', but not too much or you'll get 'Peter Panning'. Defaults to 0.0001.
SGD_API void SGD_DECL sgd_SetCSMDepthBias(float bias);

//! Set point light shadow map texture size. Defaults to 1024, must be a power of 2.
SGD_API void SGD_DECL sgd_SetPSMTextureSize(int textureSize);

//! Set max shadow casting point lights. Defaults to 32, mus be <= max points lights (32).
SGD_API void SGD_DECL sgd_SetMaxPSMLights(int maxLights);

//! Set near clip plane distance for PSM shadows. Default to .01.
SGD_API void SGD_DECL sgd_SetPSMClipNear(float near);

//! Set depth bias for PSM shadows, increase to reduce 'shadow acne', but not too much or you'll get 'Peter Panning'. Defaults to 0.0001.
SGD_API void SGD_DECL sgd_SetPSMDepthBias(float bias);

//! Set spot light shadow map texture size. Defaults to 1024, must be a power of 2.
SGD_API void SGD_DECL sgd_SetSSMTextureSize(int textureSize);

//! Set max shadow casting spot lights. Defaults to 16, must be <= max directional lights (16).
SGD_API void SGD_DECL sgd_SetMaxSSMLights(int maxLights);

//! Set near clip plane distance for SSM shadows. Default to .01.
SGD_API void SGD_DECL sgd_SetSSMClipNear(float near);

//! Set depth bias for SSM shadows, increase to reduce 'shadow acne', but not too much or you'll get 'Peter Panning'. Defaults to 0.0001.
SGD_API void SGD_DECL sgd_SetSSMDepthBias(float bias);

//! Render scene.
SGD_API void SGD_DECL sgd_RenderScene();

//! Present.
SGD_API void SGD_DECL sgd_Present();

//! Return frames per second.
SGD_API float SGD_DECL sgd_GetFPS();

//! Return renders per second.
SGD_API float SGD_DECL sgd_GetRPS();

//! @}

//! @name Entity
//! @{

//! Enable or diasable entity.
SGD_API void SGD_DECL sgd_SetEntityEnabled(SGD_Entity entity, SGD_Bool enabled);

//! Get entity enabled state. An entity is only enabled in the scene if both it and all its ancestors are enabled.
SGD_API SGD_Bool SGD_DECL sgd_IsEntityEnabled(SGD_Entity entity);

//! Show or hide entity.
SGD_API void SGD_DECL sgd_SetEntityVisible(SGD_Entity entity, SGD_Bool visible);

//! Get entity visible state. An entity is only visible in the scene if both it and all its ancestors are visible.
SGD_API SGD_Bool SGD_DECL sgd_IsEntityVisible(SGD_Entity entity);

//! Destroy entity and children recursively.
SGD_API void SGD_DECL sgd_DestroyEntity(SGD_Entity entity);

//! Reset entity collision state.
SGD_API void SGD_DECL sgd_ResetEntity(SGD_Entity entity);

//! Copy entity and children recursively.
SGD_API SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity entity);

//! Set entity name
SGD_API void SGD_DECL sgd_SetEntityName(SGD_Entity entity, SGD_String name);

//! Get entity name. The returned string will remain valid until the next call to sgd_GetEntityName.
SGD_API SGD_String SGD_DECL sgd_GetEntityName(SGD_Entity entity);

//! Set entity's parent.
SGD_API void SGD_DECL sgd_SetEntityParent(SGD_Entity entity, SGD_Entity parent);

//! Get entity's parent.
SGD_API SGD_Entity SGD_DECL sgd_GetEntityParent(SGD_Entity entity);

//! Get entity number of children
SGD_API int SGD_DECL sgd_GetEntityChildCount(SGD_Entity entity);

//! Get entity child by index. childIndex must be >= 0 and < sgd_GetEntityChildCount(entity)
SGD_API SGD_Entity SGD_DECL sgd_GetEntityChild(SGD_Entity entity, int childIndex);

//! Recursively search for an entity by name.
SGD_API SGD_Entity SGD_DECL sgd_FindEntityChild(SGD_Entity entity, SGD_String childName);

//! Set entity's world space position.
SGD_API void SGD_DECL sgd_SetEntityPosition(SGD_Entity entity, SGD_Real tx, SGD_Real ty, SGD_Real tz);

//! Set entity's world space rotation.
SGD_API void SGD_DECL sgd_SetEntityRotation(SGD_Entity entity, SGD_Real rx, SGD_Real ry, SGD_Real rz);

//! Set entity world space scale.
SGD_API void SGD_DECL sgd_SetEntityScale(SGD_Entity entity, SGD_Real sx, SGD_Real sy, SGD_Real sz);

//! Translate entity in world space.
SGD_API void SGD_DECL sgd_TranslateEntity(SGD_Entity entity, SGD_Real tx, SGD_Real ty, SGD_Real tz);

//! Rotate entity in world space.
SGD_API void SGD_DECL sgd_RotateEntity(SGD_Entity entity, SGD_Real rx, SGD_Real ry, SGD_Real rz);

//! Scale entity in world space.
SGD_API void SGD_DECL sgd_ScaleEntity(SGD_Entity entity, SGD_Real sx, SGD_Real sy, SGD_Real sz);

//! Move entity relative to it's current orientation.
SGD_API void SGD_DECL sgd_MoveEntity(SGD_Entity entity, SGD_Real tx, SGD_Real ty, SGD_Real tz);

//! Turn entity relative to it's current orientation.
SGD_API void SGD_DECL sgd_TurnEntity(SGD_Entity entity, SGD_Real rx, SGD_Real ry, SGD_Real rz);

//! Get the X component of entity's world space position.
SGD_API SGD_Real SGD_DECL sgd_GetEntityX(SGD_Entity entity);

//! Get the Y component of entity's world space position.
SGD_API SGD_Real SGD_DECL sgd_GetEntityY(SGD_Entity entity);

//! Get the Z component of entity's world space position.
SGD_API SGD_Real SGD_DECL sgd_GetEntityZ(SGD_Entity entity);

//! Get the X rotation component (ie: 'pitch') of entity's world space orientation.
SGD_API SGD_Real SGD_DECL sgd_GetEntityRX(SGD_Entity entity);

//! Get the Y rotation component (ie: 'yaw') of entity's world space orientation.
SGD_API SGD_Real SGD_DECL sgd_GetEntityRY(SGD_Entity entity);

//! Get the Z rotation component (ie: 'roll') of entity's world space orientation.
SGD_API SGD_Real SGD_DECL sgd_GetEntityRZ(SGD_Entity entity);

//! Get the X component of entity's world space scale factor.
SGD_API SGD_Real SGD_DECL sgd_GetEntitySX(SGD_Entity entity);

//! Get the Y component of entity's world space scale factor.
SGD_API SGD_Real SGD_DECL sgd_GetEntitySY(SGD_Entity entity);

//! Get the Z component of entity's world space scale factor.
SGD_API SGD_Real SGD_DECL sgd_GetEntitySZ(SGD_Entity entity);

//! Get the X component of entity's world space basis matrix right vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityIX(SGD_Entity entity);

//! Get the Y component of entity's world space basis matrix right vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityIY(SGD_Entity entity);

//! Get the Z component of entity's world space basis matrix right vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityIZ(SGD_Entity entity);

//! Get the X component of entity's world space basis matrix up vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityJX(SGD_Entity entity);

//! Get the Y component of entity's world space basis matrix up vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityJY(SGD_Entity entity);

//! Get the Z component of entity's world space basis matrix up vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityJZ(SGD_Entity entity);

//! Get the X component of entity's world space basis matrix forward vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityKX(SGD_Entity entity);

//! Get the Y component of entity's world space basis matrix forward vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityKY(SGD_Entity entity);

//! Get the Z component of entity's world space basis matrix forward vector.
SGD_API SGD_Real SGD_DECL sgd_GetEntityKZ(SGD_Entity entity);

//! Aim entity at entity.
SGD_API void SGD_DECL sgd_AimEntityAtEntity(SGD_Entity entity, SGD_Entity target, float roll);

//! Aim entity at point in world space.
SGD_API void SGD_DECL sgd_AimEntityAtPoint(SGD_Entity entity, SGD_Real x, SGD_Real y, SGD_Real z, float roll);

//! Transform 3d point from one entity coordinate space to another.
SGD_API void SGD_DECL sgd_TFormPoint(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity srcEntity, SGD_Entity dstEntity);

//! Transform 3d vector from one entity coordinate space to another.
SGD_API void SGD_DECL sgd_TFormVector(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity srcEntity, SGD_Entity dstEntity);

//! Transform 3d normal from one entity coordinate space to another.
SGD_API void SGD_DECL sgd_TFormNormal(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity srcEntity, SGD_Entity dstEntity);

//! X component of most recent sgd_TransfromPoint result.
SGD_API SGD_Real SGD_DECL sgd_GetTFormedX();

//! Y component of most recent sgd_TransfromPoint result.
SGD_API SGD_Real SGD_DECL sgd_GetTFormedY();

//! Z component of most recent sgd_TransfromPoint result.
SGD_API SGD_Real SGD_DECL sgd_GetTFormedZ();

//! @}

//! @name Camera
//! @{

//! Create a perspective camera.
SGD_API SGD_Camera SGD_DECL sgd_CreatePerspectiveCamera();

//! Create an orthographic camera.
SGD_API SGD_Camera SGD_DECL sgd_CreateOrthographicCamera();

//! Set camera vertical field of view in degrees. Defaults to 45.
SGD_API void SGD_DECL sgd_SetCameraFOV(SGD_Camera camera, float fovY);

//! Set camera near clipping plane. Defaults to 0.125.
SGD_API void SGD_DECL sgd_SetCameraNear(SGD_Camera camera, float near);

//! Set camera far clipping plane. Defaults to 1024.
SGD_API void SGD_DECL sgd_SetCameraFar(SGD_Camera camera, float far);

//! Project 3d point in world space to window coordinates.
SGD_API SGD_Bool SGD_DECL sgd_CameraProject(SGD_Camera camera, SGD_Real x, SGD_Real y, SGD_Real z);

//! X coordinate of projected point in window coordinates.
SGD_API float SGD_DECL sgd_GetProjectedX();

//! Y coordinate of projected point in window coordinates.
SGD_API float SGD_DECL sgd_GetProjectedY();

//! Unproject 3d point in wndow coordinates to world coordinates.
SGD_API SGD_Bool SGD_DECL sgd_CameraUnproject(SGD_Camera camera, float windowX, float windowY, float viewZ);

//! X coordinate of projected point in window coordinates.
SGD_API SGD_Real SGD_DECL sgd_GetUnprojectedX();

//! Y coordinate of projected point in window coordinates.
SGD_API SGD_Real SGD_DECL sgd_GetUnprojectedY();

//! Z coordinate of projected point in window coordinates.
SGD_API SGD_Real SGD_DECL sgd_GetUnprojectedZ();

//! @}

//! @name Light
//! @{

//! Create a directional light. Max directional lights is currently hardcoded at 4.
SGD_API SGD_Light SGD_DECL sgd_CreateDirectionalLight();

//! Create a point light. Max point lights is currently hardcoded at 32.
SGD_API SGD_Light SGD_DECL sgd_CreatePointLight();

//! Create a spot light. Max spot lights is currently hardcodeded at 16.
SGD_API SGD_Light SGD_DECL sgd_CreateSpotLight();

//! Set a light's color.
SGD_API void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);

//! Set a point/spot light's range.
SGD_API void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);

//! Set a point/spot light's falloff.
SGD_API void SGD_DECL sgd_SetLightFalloff(SGD_Light light, float falloff);

//! Set a spot light's inner cone angle in degrees.
SGD_API void SGD_DECL sgd_SetLightInnerConeAngle(SGD_Light light, float angle);

//! Set a spot light's outer cone angle in degrees.
SGD_API void SGD_DECL sgd_SetLightOuterConeAngle(SGD_Light light, float angle);

//! Set light shadow mapping enabled, defaults to false.
SGD_API void SGD_DECL sgd_SetLightShadowsEnabled(SGD_Light light, SGD_Bool enabled);

//! Get light shadow mapping enabled.
SGD_API SGD_Bool SGD_DECL sgd_IsLightShadowsEnabled(SGD_Light light);

//! Set light priority.
SGD_API void SGD_DECL sgd_SetLightPriority(SGD_Light light, int priority);

//! @}

//! @name Model
//! @{

//! @cond Animation mode constants used by sgd_AnimateModel
#define SGD_ANIMATION_MODE_ONE_SHOT 1
#define SGD_ANIMATION_MODE_LOOP 2
#define SGD_ANIMATION_MODE_PING_PONG 3
//! @endcond

//! Load a model.
SGD_API SGD_Model SGD_DECL sgd_LoadModel(SGD_String path);

//! Load a boned model.
SGD_API SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path, SGD_Bool skinned);

//! Create a new model.
SGD_API SGD_Model SGD_DECL sgd_CreateModel(SGD_Mesh mesh);

//! Set model mesh.
SGD_API void SGD_DECL sgd_SetModelMesh(SGD_Model model, SGD_Mesh mesh);

//! Get model mesh.
SGD_API SGD_Mesh SGD_DECL sgd_GetModelMesh(SGD_Model model);

//! Set model color.
SGD_API void SGD_DECL sgd_SetModelColor(SGD_Model model, float red, float green, float blue, float alpha);

//! Animate a model.
//!
//! `mode` should be one of the following:
//!
//! Animation mode              | Integer value | meaning
//! ----------------------------|---------------|--------
//! SGD_ANIMATION_MODE_ONE_SHOT | 1             | Animation plays forward once then ends.
//! SGD_ANIMATION_MODE_LOOP     | 2             | Animation plays forward repeatedly.
//! SGD_ANIMATION_PING_PING     | 3             | Animation plays forward then backwards repeatedly.
SGD_API void SGD_DECL sgd_AnimateModel(SGD_Model model, int animation, float time, int mode, float weight);

//! @}

//! @name Skybox
//! @{

//! Load a skybox.
SGD_API SGD_Skybox SGD_DECL sgd_LoadSkybox(SGD_String path, float roughness);

//! Create a new skybox.
SGD_API SGD_Skybox SGD_DECL sgd_CreateSkybox(SGD_Texture texture);

//! Set skybox texture.
SGD_API void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox skybox, SGD_Texture texture);

//! Set skybox roughness in the range 0 to 1.
SGD_API void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox skybox, float roughness);

//! @}

//! @name Sprite
//! @{

//! Create a new sprite.
SGD_API SGD_Sprite SGD_DECL sgd_CreateSprite(SGD_Image image);

//! Set sprite material.
SGD_API void SGD_DECL sgd_SetSpriteImage(SGD_Sprite sprite, SGD_Image image);

//! Set sprite color.
SGD_API void SGD_DECL sgd_SetSpriteColor(SGD_Sprite sprite, float red, float green, float blue, float alpha);

//! Set sprite animation frame.
SGD_API void SGD_DECL sgd_SetSpriteFrame(SGD_Sprite sprite, float frame);

//! @}

//! @name Collisions
//! @{

//! @cond
#define SGD_COLLISION_RESPONSE_NONE 0
#define SGD_COLLISION_RESPONSE_STOP 1
#define SGD_COLLISION_RESPONSE_SLIDE 2
#define SGD_COLLISION_RESPONSE_SLIDEXZ 3
//! @endcond

//! Create a new sphere collider and attach it to entity.
SGD_API SGD_Collider SGD_DECL sgd_CreateSphereCollider(SGD_Entity entity, int colliderType, float radius);

//! Create a new ellipsoid collider and attach it to entity.
SGD_API SGD_Collider SGD_DECL sgd_CreateEllipsoidCollider(SGD_Entity entity, int colliderType, float radius, float height);

//! Create a new mesh collider and attach it to entity.
SGD_API SGD_Collider SGD_DECL sgd_CreateMeshCollider(SGD_Entity entity, int colliderType, SGD_Mesh mesh);

//! Return entity a collider is attached to.
SGD_API SGD_Entity SGD_DECL sgd_GetColliderEntity(SGD_Collider collider);

//! Set sphere or ellipsoid collider radius.
SGD_API void SGD_DECL sgd_SetColliderRadius(SGD_Collider collider, float radius);

//! Set ellipsoid collider height.
SGD_API void SGD_DECL sgd_SetColliderHeight(SGD_Collider collider, float height);

//! Enable collisons between 2 collider types.
//!
//! `response` should be one of the following:
//!
//! Response                       | Integer value | Description
//! -------------------------------|---------------|------------
//! SGD_COLLISION_RESPONSE_NONE    | 0             | Source collider should do nothing when it collides with destination collider.
//! SGD_COLLISION_RESPONSE_STOP    | 1             | Source collider should stop when it collides with destination collider.
//! SGD_COLLISION_RESPONSE_SLIDE   | 2             | Source collider should slide along geometry when it collides with destination collider.
//! SGD_COLLISION_RESPONSE_SLIDEXZ | 3             | Source collider should slide along XZ plane when it collides with destination collider.
SGD_API void SGD_DECL sgd_EnableCollisions(int srcColliderType, int dstColliderType, int response);

//! Update enabled colliders.
SGD_API void SGD_DECL sgd_UpdateColliders();

//! Get collisions generated by sgd_UpdateColliders for moving collider.
SGD_API int SGD_DECL sgd_GetCollisionCount(SGD_Collider collider);

//! Get collision X coordinate.
SGD_API SGD_Real SGD_DECL sgd_GetCollisionX(SGD_Collider collider, int index);

//! Get collision Y coordinate.
SGD_API SGD_Real SGD_DECL sgd_GetCollisionY(SGD_Collider collider, int index);

//! Get collision Z coordinate.
SGD_API SGD_Real SGD_DECL sgd_GetCollisionZ(SGD_Collider collider, int index);

//! Get collision normal X component.
SGD_API SGD_Real SGD_DECL sgd_GetCollisionNX(SGD_Collider collider, int index);

//! Get collision normal Y component.
SGD_API SGD_Real SGD_DECL sgd_GetCollisionNY(SGD_Collider collider, int index);

//! Get collision normal Z component.
SGD_API SGD_Real SGD_DECL sgd_GetCollisionNZ(SGD_Collider collider, int index);

//! @}

//! @name Collider picking
//! @{

//! Pick first collider along ray passing from camera eye through window coordinates.
SGD_API SGD_Collider SGD_DECL sgd_CameraPick(SGD_Camera camera, float windowX, float windowY, int colliderMask);

//! Pick first collider along line.
SGD_API SGD_Collider SGD_DECL sgd_LinePick(SGD_Real x0, SGD_Real y0, SGD_Real z0, SGD_Real x1, SGD_Real y1, SGD_Real z1, SGD_Real radius, int colliderMask);

//! X coordinate of pick contact point in world space.
SGD_API SGD_Real SGD_DECL sgd_GetPickedX();

//! Y coordinate of pick contact point in world space.
SGD_API SGD_Real SGD_DECL sgd_GetPickedY();

//! Z coordinate of pick contact point in world space.
SGD_API SGD_Real SGD_DECL sgd_GetPickedZ();

//! X compnent of pick contact normal in world space.
SGD_API SGD_Real SGD_DECL sgd_GetPickedNX();

//! Y component of pick contact normal in world space.
SGD_API SGD_Real SGD_DECL sgd_GetPickedNY();

//! Z component of pick contact normal in world space.
SGD_API SGD_Real SGD_DECL sgd_GetPickedNZ();

//! @}

//! @name Render effects
//! @{

//! Create a new bloom effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateBloomEffect();

//! Create a new blur render effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateBlurEffect();

// Set blur render effect radius, should be in the range 1 to 31 inclusive, defaults to 2.
SGD_API void SGD_DECL sgd_SetBlurEffectRadius(SGD_RenderEffect effect, int radius);

//! Create a new fog render effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateFogEffect();

// Set fog render effect color. Defaults to .8, .9, 1, 1
SGD_API void SGD_DECL sgd_SetFogEffectColor(SGD_RenderEffect effect, float red, float green, float blue, float alpha);

// Set fog render effect range. Defaults to 0, 1024.
SGD_API void SGD_DECL sgd_SetFogEffectRange(SGD_RenderEffect effect, float near, float far);

// Set fog render effect power. Defaults to 2.
SGD_API void SGD_DECL sgd_SetFogEffectPower(SGD_RenderEffect effect, float power);

//! Create a new monocolor render effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateMonocolorEffect();

//! Set monocolor render effect color, defaults to 1,1,1,1.
SGD_API void SGD_DECL sgd_SetMonocolorEffectColor(SGD_RenderEffect effect, float red, float green, float blue, float alpha);

//! Set render effect enabled.
SGD_API void SGD_DECL sgd_SetRenderEffectEnabled(SGD_RenderEffect effect, SGD_Bool enabled);

//! Is render effect enabled.
SGD_API SGD_Bool SGD_DECL sgd_IsRenderEffectEnabled(SGD_RenderEffect effect);

//! @}

//! @name ImGui
//! @{

#if SGD_DYNAMIC
//! @cond Private internal ImGui interface used by sgd/imgui_impl_sgd.h glue
SGD_API SGD_Bool SGD_DECL sgd_ImGui_ImplSGD_Init(void* imguiProcs);
SGD_API void SGD_DECL sgd_ImGui_ImplSGD_Shutdown();
SGD_API void SGD_DECL sgd_ImGui_ImplSGD_NewFrame();
SGD_API void SGD_DECL sgd_ImGui_ImplSGD_RenderDrawData(void* imguiDrawData);
//! @endcond
#endif

//! @}

#endif
