#ifndef SGD_SGD_H_INCLUDED
#define SGD_SGD_H_INCLUDED

#include "keycodes.h"

//! @cond

#define SGD_VERSION_MAJOR 0
#define SGD_VERSION_MINOR 16
#define SGD_VERSION_PATCH 0
#define SGD_VERSION "0.16.0" // How to macro-ize this?

#if defined(SWIG) || defined(SGD_GENAPI)

#define SGD_API
#define SGD_DECL

#else

#include <stdint.h>

//! @file
//!
//! @mainpage
//!
//! Welcome to the LibSGD reference documentation.
//!
//! Quick links: sgd.h keycodes.h

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

#endif

//! @endcond

//! @defgroup CoreTypes CoreTypes
//! @{

//! Boolean type. Non-0 values indicate true, 0 indicates false.
typedef int SGD_Bool;

//! Flags type used to pass bitmask style enums to functions.
typedef int SGD_Flags;

//! Opaque handle type used to reference SGD objects.
typedef int SGD_Handle;

//! String type. Strings are assumed to be UTF-8 encoded.
typedef const char* SGD_String;

#if defined(SWIG) || defined(SGD_GENAPI) || UINTPTR_MAX == 0xffffffffffffffff
//! Floating point type.
typedef double SGD_Real;
#else
//! Flat point type.
typedef float SGD_Real;
#endif

//! @}

//! @defgroup CoreConsts CoreConsts
//! @{

static const int SGD_NULL = 0;
static const int SGD_TRUE = 1;
static const int SGD_FALSE = 0;

//! @}

//! @defgroup SystemTypes SystemTypes
//! @{

//! Events returned by sgd_PollEvents.
typedef enum SGD_EventMask {
	SGD_EVENT_MASK_CLOSE_CLICKED = 0x01, //!< Window close clicked.
	SGD_EVENT_MASK_SIZE_CHANGED = 0x02,	 //!< Window size changed.
	SGD_EVENT_MASK_LOST_FOCUS = 0x04,	 //!< Window lost focus.
	SGD_EVENT_MASK_GOT_FOCUS = 0x08,	 //!< Window got focus.
	SGD_EVENT_MASK_SUSPENDED = 0x10,	 //!< App suspended.
	SGD_EVENT_MASK_RESUMED = 0x20		 //!< App resumed.
} SGD_EventMask;

//! @}

//! @defgroup System System
//! @{

//! Start up libsgd.
SGD_API void SGD_DECL sgd_Init();

//! Get current runtime LibSGD version.
//!
//! Returns the value of SGD_VERSION macro at the time the runtime was built.
SGD_API SGD_String SGD_DECL sgd_GetVersion();

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
//! dawn.backendType          |         | Backend for Dawn to use, one of: "D3D12", "D3D11", "Vulkan", "Metal". Defaults to
//! "D3D12" on 64 bit Windows, "D3D11" on 32 bit Windows, "Vulkan" on 64 bit Linux and "Metal" on 64 bit MacOS.
//! render.vsyncEnabled       | "1"     | Hack that fixes https://issues.chromium.org/issues/42241496#comment3
//! render.shadowPassEnabled  | "1"     | Enables shadow rendering.
//! render.opaquePassEnabled  | "1"     | Enables opaque rendering.
//! render.blendPassEnabled   | "1"     | Enables blended rendering.
//! render.effectPassEnabled  | "1"     | Enables effects rendering.
SGD_API void SGD_DECL sgd_SetConfigVar(SGD_String name, SGD_String value);

//! Set global configuration variable. The returned value is valid until the next call to sgd_GetConfigVar.
SGD_API SGD_String SGD_DECL sgd_GetConfigVar(SGD_String name);

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

//! Poll system for events, returns a bit mask of event types that occured.
//!
//! The returned return value should be bitwise 'and'ed with one of the @ref SGD_EventMask
//! values to determine whether a particular event occured or not.
SGD_API SGD_EventMask SGD_DECL sgd_PollEvents();

//! Release an object handle.
//!
//! Will cause the object referenced by the handle to become available for destruction,
//! and will also invalidate the handle.
//!
//! An object will not be destroyed while it is still being used by another object, eg:
//! a model's mesh will not be destroyed while it is still being used by a Model.
//!
//! It is an error to use an invalidated handle, so once a handle is released be careful
//! not to use it again.
SGD_API void SGD_DECL sgd_ReleaseHandle(SGD_Handle handle);

//! Release all object handles.
SGD_API void SGD_DECL sgd_ReleaseAllHandles();

//! @cond Debug memory state.
SGD_API void SGD_DECL sgd_DebugMemory();
//! @endcond

//! @}

//! @defgroup WindowTypes WindowTypes
//! @{

//! Window states.
typedef enum SGD_WindowState {
	SGD_WINDOW_STATE_CLOSED = 0,	//!< Window is closed
	SGD_WINDOW_STATE_MINIMIZED = 1, //!< Window is minimized
	SGD_WINDOW_STATE_NORMAL = 2,	//!< Window is normal
	SGD_WINDOW_STATE_MAXIMIZED = 3, //!< Window is maximized
	SGD_WINDOW_STATE_FULLSCREEN = 4 //!< Window is fullscreen
} SGD_WindowState;

//! Window flags.
typedef enum SGD_WindowFlags {
	SGD_WINDOW_FLAGS_NONE = 0x00,		//!< No special flags.
	SGD_WINDOW_FLAGS_FULLSCREEN = 0x01, //!< Create fullscreen window.
	SGD_WINDOW_FLAGS_RESIZABLE = 0x02,	//!< Create resizable window.
	SGD_WINDOW_FLAGS_CENTERED = 0x04	//!< Create window centered on desktop.
} SGD_WindowFlags;

//! @}

//! @defgroup Window Window
//! @{

//! Create a new window. See @ref SGD_WindowFlags for possible values for `flags`.
SGD_API void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, SGD_Flags flags);

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
SGD_API void SGD_DECL sgd_SetWindowState(SGD_WindowState state);

//! Get window state. See sgd_SetWindowState.
SGD_API SGD_WindowState SGD_DECL sgd_GetWindowState();

//! @}

//! @defgroup InputTypes InputTypes
//! @{

//! Mouse cursor modes.
typedef enum SGD_MouseCursorMode {
	SGD_MOUSE_CURSOR_MODE_NORMAL = 1,
	SGD_MOUSE_CURSOR_MODE_HIDDEN = 2,
	SGD_MOUSE_CURSOR_MODE_DISABLED = 3,
	SGD_MOUSE_CURSOR_MODE_CAPTURED = 4
} SGD_MouseCursorMode;

//! @}

//! @defgroup Input Input
//! @{

//! True if key is currently held down.
SGD_API SGD_Bool SGD_DECL sgd_IsKeyDown(SGD_Key key);

//! True if key was pressed down last frame.
SGD_API SGD_Bool SGD_DECL sgd_IsKeyHit(SGD_Key key);

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

//! Set mouse cursor mode.
SGD_API void SGD_DECL sgd_SetMouseCursorMode(SGD_MouseCursorMode cursorMode);

//! True if mouse button is curently held down.
SGD_API SGD_Bool SGD_DECL sgd_IsMouseButtonDown(SGD_MouseButton button);

//! True if mouse button was pressed down last frame.
SGD_API SGD_Bool SGD_DECL sgd_IsMouseButtonHit(SGD_MouseButton button);

//! True if gamepad is currently connected.
SGD_API SGD_Bool SGD_DECL sgd_IsGamepadConnected(int gamepad);

//! True if gamepad button currently held down.
SGD_API SGD_Bool SGD_DECL sgd_IsGamepadButtonDown(int gamepad, SGD_GamepadButton button);

//! True if gamepad button was pressed down last frame.
SGD_API SGD_Bool SGD_DECL sgd_IsGamepadButtonHit(int gamepad, SGD_GamepadButton button);

//! Value in the range -1 to 1 representing joystick axis position.
SGD_API float SGD_DECL sgd_GetGamepadAxis(int gamepad, SGD_GamepadAxis axis);

//! @}

//! @defgroup TextureTypes TextureTypes
//! @{

//! Texture handle type.
typedef SGD_Handle SGD_Texture;

//! Texture types.
typedef enum SGD_TextureType {
	SGD_TEXTURE_TYPE_2D = 1,
	SGD_TEXTURE_TYPE_CUBE = 2,
	SGD_TEXTURE_TYPE_ARRAY = 3,
} SGD_TextureType;

//! Texture formats.
//!
//! The 8 bit unsigned, normalized formats are converted to values in the range 0.0..1.0 in shader code.
//!
//! The 8 bit signed, normalized formats are converted to values in the range -1.0..+1.0 in shader code.
//!
//! All formats except for SGD_TEXTURE_FORMAT_SRGBA8 represent values in 'linear color space'.
//!
//! SGD_TEXTURE_FORMAT_ANY can be used with loading functions to let SGD pick a suitable texture format
//! depending on the content of the data being loaded.
typedef enum SGD_TextureFormat {
	SGD_TEXTURE_FORMAT_ANY = 0, //!< Let SGD loaders choose format.

	SGD_TEXTURE_FORMAT_R8 = 1,	   //!< 8 bit unsigned normalized red.
	SGD_TEXTURE_FORMAT_RG8 = 2,	   //!< 8 bit unsigned normalized red, green.
	SGD_TEXTURE_FORMAT_RGBA8 = 3,  //!< 8 bit unsigned normalized red, green, blue, alpha.
	SGD_TEXTURE_FORMAT_SRGBA8 = 4, //!< 8 bit unsigned normalized red, green, blue, alpha. Non-linear

	SGD_TEXTURE_FORMAT_R8S = 5,	   //!< 8 bit signed normalized red.
	SGD_TEXTURE_FORMAT_RG8S = 6,   //!< 8 bit signed normalized red, green.
	SGD_TEXTURE_FORMAT_RGBA8S = 7, //!< 8 bit signed normalized red, green, blue, alpha.

	SGD_TEXTURE_FORMAT_R16F = 8,	 //!< 16 bit floating point red only.
	SGD_TEXTURE_FORMAT_RG16F = 9,	 //!< 16 bit floating point red only.
	SGD_TEXTURE_FORMAT_RGBA16F = 10, //!< 16 bit floating point red only.
} SGD_TextureFormat;

//! Texture flags
//!
//! By default, texture coordinates wrap at the edges of textures.
//!
//! Textures are always linear filtered whe minimized.
typedef enum SGD_TextureFlags {
	SGD_TEXTURE_FLAGS_NONE = 0x0,	  //!< No texture flags.
	SGD_TEXTURE_FLAGS_CLAMP_U = 0x01, //!< Clamp texture U coordinates.
	SGD_TEXTURE_FLAGS_CLAMP_V = 0x02, //!< Clamp texture V coordinates.
	SGD_TEXTURE_FLAGS_CLAMP_W = 0x04, //!< Clamp texture W coordinates.
	SGD_TEXTURE_FLAGS_FILTER = 0x08,  //!< Perform bilinear filtering on texels that cover more than screen pixel.
	SGD_TEXTURE_FLAGS_MIPMAP = 0x10,  //!< Create and perform mipmapping.
	SGD_TEXTURE_FLAGS_DEFAULT = 0x18, //!< Combination of SGD_TEXTURE_FLAGS_FILTER and SGD_TEXTURE_FLAGS_MIPMAP
	SGD_TEXTURE_FLAGS_IMAGE = 0x01f, //!< Combination of SGD_TEXTURE_FLAGS_FILTER, SGD_TEXTURE_FLAGS_MIPMAP and all clamp flags.
} SGD_TextureFlags;

//! @}

//! @defgroup Texture Texture
//! @{

//! Load a new 2D texture. See also @ref SGD_TextureFlags.
//!
//! @param path is the file path of the texture to load.
//! @param format is a SGD_TextureFormat constant.
//! @param flags is a valid combination of SGD_TextureFlags values.
SGD_API SGD_Texture SGD_DECL sgd_Load2DTexture(SGD_String path, SGD_TextureFormat format, SGD_Flags flags);

//! Load a new cube texture. See also @ref SGD_TextureFlags.
SGD_API SGD_Texture SGD_DECL sgd_LoadCubeTexture(SGD_String path, SGD_TextureFormat format, SGD_Flags flags);

//! Load a new array texture. See also @ref SGD_TextureFlags.
SGD_API SGD_Texture SGD_DECL sgd_LoadArrayTexture(SGD_String path, SGD_TextureFormat format, SGD_Flags flags);

//! Create a new 2D texture. See also @ref SGD_TextureFlags.
SGD_API SGD_Texture SGD_DECL sgd_Create2DTexture(int width, int height, SGD_TextureFormat format, SGD_Flags flags);

//! Create a new cube texture. See also @ref SGD_TextureFlags.
SGD_API SGD_Texture SGD_DECL sgd_CreateCubeTexture(int size, SGD_TextureFormat format, SGD_Flags flags);

//! Create a new array texture. See also @ref SGD_TextureFlags.
SGD_API SGD_Texture SGD_DECL sgd_CreateArrayTexture(int width, int height, int depth, SGD_TextureFormat format,
													SGD_Flags flags);

//! Get texture type.
SGD_API SGD_TextureType SGD_DECL sgd_GetTextureType(SGD_Texture texture);

//! Get width of texture.
SGD_API int SGD_DECL sgd_GetTextureWidth(SGD_Texture texture);

//! Get height of texture.
SGD_API int SGD_DECL sgd_GetTextureHeight(SGD_Texture texture);

//! Get height of texture.
SGD_API int SGD_DECL sgd_GetTextureDepth(SGD_Texture texture);

//! Get texture format.
SGD_API SGD_TextureFormat SGD_DECL sgd_GetTextureFormat(SGD_Texture texture);

//! Get texture flags.
SGD_API SGD_TextureFlags SGD_DECL sgd_GetTextureFlags(SGD_Texture texture);

//! Set texture texel.
SGD_API void SGD_DECL sgd_SetTexelSRGBA(SGD_Texture texture, int x, int y, int rgba);

//! Get texture texel
SGD_API int SGD_DECL sgd_GetTexelSRGBA(SGD_Texture texture, int x, int y);

//! @}

//! @defgroup MaterialTypes MaterialTypes
//! @{

//! Material handle type.
typedef SGD_Handle SGD_Material;

//! Blend mode.
typedef enum SGD_BlendMode {
	SGD_BLEND_MODE_OPAQUE = 1,
	SGD_BLEND_MODE_ALPHA_MASK = 2,
	SGD_BLEND_MODE_ALPHA_BLEND = 3,
} SGD_BlendMode;

//! Depth Comparison function.
typedef enum SGD_DepthFunc {
	SGD_DEPTH_FUNC_NEVER = 1,
	SGD_DEPTH_FUNC_LESS = 2,
	SGD_DEPTH_FUNC_EQUAL = 3,
	SGD_DEPTH_FUNC_LESS_EQUAL = 4,
	SGD_DEPTH_FUNC_GREATER = 5,
	SGD_DEPTH_FUNC_NOT_EQUAL = 6,
	SGD_DEPTH_FUNC_GREATER_EQUAL = 7,
	SGD_DEPTH_FUNC_ALWAYS = 8,
} SGD_DepthFunc;

//! Cull modes.
typedef enum SGD_CullMode {
	SGD_CULL_MODE_NONE = 1,
	SGD_CULL_MODE_FRONT = 2,
	SGD_CULL_MODE_BACK = 3,
} SGD_CullMode;

//! @}

//! @defgroup Material Material
//! @{

//! Load a new PBR material.
SGD_API SGD_Material SGD_DECL sgd_LoadPBRMaterial(SGD_String path);

//! Create a new PBR material.
SGD_API SGD_Material SGD_DECL sgd_CreatePBRMaterial();

//! Load a new prelit material.
SGD_API SGD_Material SGD_DECL sgd_LoadPrelitMaterial(SGD_String path);

//! Create a new prelit material.
SGD_API SGD_Material SGD_DECL sgd_CreatePrelitMaterial();

//! Set material blend mode.
SGD_API void SGD_DECL sgd_SetMaterialBlendMode(SGD_Material material, SGD_BlendMode blendMode);

//! Set material depth comparison function.
SGD_API void SGD_DECL sgd_SetMaterialDepthFunc(SGD_Material material, SGD_DepthFunc depthFunc);

//! Set material cull mode.
SGD_API void SGD_DECL sgd_SetMaterialCullMode(SGD_Material material, SGD_CullMode cullMode);

//! Set material texture parameter.
SGD_API void SGD_DECL sgd_SetMaterialTexture(SGD_Material material, SGD_String parameter, SGD_Texture texture);

//! Set material color parameter.
SGD_API void SGD_DECL sgd_SetMaterialColor(SGD_Material material, SGD_String parameter, float red, float green, float blue,
										   float alpha);

//! Set material float parameter.
SGD_API void SGD_DECL sgd_SetMaterialFloat(SGD_Material material, SGD_String parameter, float value);

//! @}

//! @defgroup MeshTypes MeshTypes
//! @{

//! Mesh flags.
typedef enum SGD_MeshFlags {
	SGD_MESH_FLAGS_NONE = 0,			 //!< No special mesh flags.
	SGD_MESH_FLAGS_TANGENTS_ENABLED = 1, //!< Mesh contains materials with normal maps.
	SGD_MESH_FLAGS_BLENDED_SURFACES = 2, //!< Mesh contains materials that use alphaBlend blend mode.
} SGD_MeshFlags;

//! @}

//! @defgroup Mesh Mesh
//! @{

//! Mesh handle type
typedef SGD_Handle SGD_Mesh;

//! Surface handle type.
typedef SGD_Handle SGD_Surface;

//! Load a new mesh.
SGD_API SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path);

//! Copy mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CopyMesh(SGD_Mesh mesh);

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
SGD_API SGD_Mesh SGD_DECL sgd_CreateTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs,
											  SGD_Material material);

//! Set mesh shadow casting enabled, defaults to true.
SGD_API void SGD_DECL sgd_SetMeshShadowsEnabled(SGD_Mesh mesh, SGD_Bool enabled);

//! Get mesh shadow casting enabled.
SGD_API SGD_Bool SGD_DECL sgd_IsMeshShadowsEnabled(SGD_Mesh mesh);

//! Update mesh normals.
SGD_API void SGD_DECL sgd_UpdateMeshNormals(SGD_Mesh mesh);

//! Update mesh tangets. Mesh must have SGD_MESH_TANGENTS_ENABLED flag.
SGD_API void SGD_DECL sgd_UpdateMeshTangents(SGD_Mesh mesh);

//! Fit mesh to a bounding box.
SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh mesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
								  SGD_Bool uniform);

//! Transform mesh by translation, rotation, scale.
SGD_API void SGD_DECL sgd_TransformMesh(SGD_Mesh mesh, float tx, float ty, float tz, float rx, float ry, float rz, float sx,
										float sy, float sz);

//! Transform mesh texture coordinates.
SGD_API void SGD_DECL sgd_TransformTexCoords(SGD_Mesh mesh, float scaleX, float scaleY, float offsetX, float offsetY);

//! Flip mesh.
SGD_API void SGD_DECL sgd_FlipMesh(SGD_Mesh mesh);

//! Get mesh bounding box min x coordinate.
SGD_API float SGD_DECL sgd_GetMeshBoundsMinX(SGD_Mesh mesh);

//! Get mesh bounding box min y coordinate.
SGD_API float SGD_DECL sgd_GetMeshBoundsMinY(SGD_Mesh mesh);

//! Get mesh bounding box min z coordinate.
SGD_API float SGD_DECL sgd_GetMeshBoundsMinZ(SGD_Mesh mesh);

//! Get mesh bounding box max x coordinate.
SGD_API float SGD_DECL sgd_GetMeshBoundsMaxX(SGD_Mesh mesh);

//! Get mesh bounding box max y coordinate.
SGD_API float SGD_DECL sgd_GetMeshBoundsMaxY(SGD_Mesh mesh);

//! Get mesh bounding box max z coordinate.
SGD_API float SGD_DECL sgd_GetMeshBoundsMaxZ(SGD_Mesh mesh);

//! Create a new custom mesh. See also @ref SGD_MeshFlags.
SGD_API SGD_Mesh SGD_DECL sgd_CreateMesh(int vertexCount, SGD_Flags flags);

//! Add uninitialized vertices to a mesh, returning index of the first new vertex.
SGD_API void SGD_DECL sgd_ResizeVertices(SGD_Mesh mesh, int count);

//! Get number of vertices in mesh.
SGD_API int SGD_DECL sgd_GetVertexCount(SGD_Mesh mesh);

//! Add a vertex to a mesh, returning index of new vertex.
SGD_API int SGD_DECL sgd_AddVertex(SGD_Mesh mesh, float x, float y, float z, float nx, float ny, float nz, float s, float t);

//! Set vertex.
SGD_API void SGD_DECL sgd_SetVertex(SGD_Mesh mesh, int vertex, float x, float y, float z, float nx, float ny, float nz, float s,
									float t);

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

//! Create a new Surface and add it to mesh.
SGD_API SGD_Surface SGD_DECL sgd_CreateSurface(SGD_Mesh mesh, SGD_Material material, int triangleCount);

//! Get mesh surface count.
SGD_API int SGD_DECL sgd_GetSurfaceCount(SGD_Mesh mesh);

//! Get mesh surface.
SGD_API SGD_Surface SGD_DECL sgd_GetSurface(SGD_Mesh mesh, int surface);

//! Get surface material.
SGD_API SGD_Material SGD_DECL sgd_GetMaterial(SGD_Surface surface);

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

//! @defgroup FontTypes FontTypes
//! @{

//! Font handle type.
typedef SGD_Handle SGD_Font;

//! @}

//! @defgroup Font Font
//! @{

//! Load a new font.
SGD_API SGD_Font SGD_DECL sgd_LoadFont(SGD_String path, float height);

//! Get width of text.
SGD_API float SGD_DECL sgd_GetTextWidth(SGD_Font font, SGD_String text);

//! get height of font.
SGD_API float SGD_DECL sgd_GetFontHeight(SGD_Font font);

//! @}

//! @defgroup ImageTypes ImageTypes
//! @{

//! Image type
typedef SGD_Handle SGD_Image;

//! @}

//! @defgroup Image Image
//! @{

//! Load an image for use with 3D sprites or Draw2DImage.
SGD_API SGD_Image SGD_DECL sgd_LoadImage(SGD_String path);

//! Load an array image for use with 3D sprites or Draw2DImage.
SGD_API SGD_Image SGD_DECL sgd_LoadArrayImage(SGD_String path, int frameCount, int framesX, int framesY, int frameSpacing);

//! Create an image with an existing texture.
SGD_API SGD_Image SGD_DECL sgd_CreateImage(SGD_Texture texture);

//! Image view modes.
typedef enum SGD_ImageViewMode {
	SGD_IMAGE_VIEW_MODE_FIXED = 1,
	SGD_IMAGE_VIEW_MODE_FREE = 2,
	SGD_IMAGE_VIEW_MODE_UPRIGHT = 3
} SGD_ImageViewMode;

//! Set view mode for use with 3D sprites.
SGD_API void SGD_DECL sgd_SetImageViewMode(SGD_Image image, SGD_ImageViewMode viewMode);

//! Set image blend mode.
SGD_API void SGD_DECL sgd_SetImageBlendMode(SGD_Image image, SGD_BlendMode blendMode);

//! Set rect for use with 3D sprites.
SGD_API void SGD_DECL sgd_SetImageRect(SGD_Image image, float minX, float minY, float maxX, float maxY);

//! Get texture used to create image.
SGD_API SGD_Texture SGD_DECL sgd_GetImageTexture(SGD_Image image);

//! @}

//! @defgroup Overlay Overlay
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
SGD_API void SGD_DECL sgd_Draw2DText(SGD_String text, float x, float y);

//! @}

//! @defgroup Scene Scene
//! @{

//! Destroy all active entities.
SGD_API void SGD_DECL sgd_ClearScene();

//! Reset scene to default state.
//!
//! Destroys all active entities in the scene.
//!
//! If the release all handles argument is true, sgd_ReleaseAllHandles is also called, allowing
//! you to reset to the initial app state with a single command.
SGD_API void SGD_DECL sgd_ResetScene(SGD_Bool releaseAllHandles);

//! Experimental JSON loader!
SGD_API void SGD_DECL sgd_LoadScene(SGD_String path);

//! Experimental JSON saver!
SGD_API void SGD_DECL sgd_SaveScene(SGD_String path);

//! Set scene ambient light color.
SGD_API void SGD_DECL sgd_SetAmbientLightColor(float red, float green, float blue, float alpha);

//! Set scene clear color.
SGD_API void SGD_DECL sgd_SetClearColor(float red, float green, float blue, float alpha);

//! Set scene clear depth.
SGD_API void SGD_DECL sgd_SetClearDepth(float depth);

//! Set scene environment texture.
SGD_API void SGD_DECL sgd_SetEnvTexture(SGD_Texture texture);

//! Update scene shadow mapping config from config vars.
//!
//! This function will update shadow mapping config variables from the current config vars, see sgd_SetConfigVar.
//!
//! Config Var         | Type  | Default                | Description
//! -------------------|-------|------------------------|------------
//! csm.textureSize    | int   | "2048"                 | Texture size of each of the 4 cascading shadow map textures.
//! csm.maxLights      | int   | "4"                    | Max directional light shadow casters.
//! csm.clipRange      | float | "330.0"                | Max offscreen distance to render shadow casters.
//! csm.depthBias      | float | "0.0001"               | Depth bias for cascading shadow maps.
//! csm.SplitDistances | Vec4f | "15,60,250,1000"       | Cascading shadow map split distances from the eye.
//! psm.textureSize    | int   | "1024"                 | Texture size of point light cube shadow textures.
//! psm.maxLights      | int   | "32"                   | Max point light shadow casters.
//! psm.clipNear       | float | "0.25"                 | Near clipping plane for point light shadow maps.
//! psm.depthBias      | float | "0.0001"               | Depth bias value for p[oint light shadow maps.
//! ssm.textureSize    | int   | "1024"                 | Texture size of each spot light 2D shadow map.
//! ssm.maxLights      | int   | "16"                   | Max spot light shadow casters.
//! ssm.clipNear       | float | "0.25"                 | Max distance to render shadow casters.
//! ssm.depthBias      | float | "0.0001"               | Depth bias value to prevent shadow acne.
SGD_API void SGD_DECL sgd_UpdateShadowMappingConfig();

//! Render scene.
SGD_API void SGD_DECL sgd_RenderScene();

//! Present.
SGD_API void SGD_DECL sgd_Present();

//! Return frames per second.
SGD_API float SGD_DECL sgd_GetFPS();

//! Return renders per second.
SGD_API float SGD_DECL sgd_GetRPS();

//! @}

//! @defgroup EntityTypes EntityTypes
//! @{

//! Entity handle type.
typedef SGD_Handle SGD_Entity;

//! @}

//! @defgroup Entity Entity
//! @{

//! Set entity enabled state. An entity is only enabled in the scene if both it and all its ancestors are enabled.
SGD_API void SGD_DECL sgd_SetEntityEnabled(SGD_Entity entity, SGD_Bool enabled);

//! Get entity enabled state.
SGD_API SGD_Bool SGD_DECL sgd_IsEntityEnabled(SGD_Entity entity);

//! Set entity visible state. An entity is only visible in the scene if both it and all its ancestors are visible.
SGD_API void SGD_DECL sgd_SetEntityVisible(SGD_Entity entity, SGD_Bool visible);

//! Get entity visible state.
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
SGD_API void SGD_DECL sgd_TransformPoint(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity srcEntity, SGD_Entity dstEntity);

//! Transform 3d vector from one entity coordinate space to another.
SGD_API void SGD_DECL sgd_TransformVector(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity srcEntity, SGD_Entity dstEntity);

//! Transform 3d normal from one entity coordinate space to another.
SGD_API void SGD_DECL sgd_TransformNormal(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity srcEntity, SGD_Entity dstEntity);

//! X component of most recent sgd_TransformPoint result.
SGD_API SGD_Real SGD_DECL sgd_GetTransformedX();

//! Y component of most recent sgd_TransformPoint result.
SGD_API SGD_Real SGD_DECL sgd_GetTransformedY();

//! Z component of most recent sgd_TransformPoint result.
SGD_API SGD_Real SGD_DECL sgd_GetTransformedZ();

//! @}

//! @defgroup CameraTypes CameraTypes
//! @{

//! Camera handle type.
typedef SGD_Entity SGD_Camera;

typedef enum SGD_CameraType {
	SGD_CAMERA_TYPE_PERSPECTIVE = 1,
	SGD_CAMERA_TYPE_ORTHOGRAPHIC = 2,
} SGD_CameraType;

//! @}

//! @defgroup Camera Camera
//! @{

//! Create a new perspective camera.
SGD_API SGD_Camera SGD_DECL sgd_CreatePerspectiveCamera();

//! Create a new orthographic camera.
SGD_API SGD_Camera SGD_DECL sgd_CreateOrthographicCamera();

//! Get camera type.
SGD_API SGD_CameraType SGD_DECL sgd_GetCameraType(SGD_Camera camera);

//! Set camera vertical field of view in degrees. Defaults to 90.
SGD_API void SGD_DECL sgd_SetCameraFOV(SGD_Camera camera, float fovY);

//! Set camera near clipping plane. Defaults to 0.1.
SGD_API void SGD_DECL sgd_SetCameraNear(SGD_Camera camera, float near);

//! Set camera far clipping plane. Defaults to 1000.
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

//! @defgroup LightTypes LightTypes
//! @{

//! Light handle type.
typedef SGD_Entity SGD_Light;

//! Light type.
typedef enum SGD_LightType {
	SGD_LIGHT_TYPE_DIRECTIONAL = 1,
	SGD_LIGHT_TYPE_POINT = 2,
	SGD_LIGHT_TYPE_SPOT = 3,
} SGD_LightType;

//! @}

//! @defgroup Light Light
//! @{

//! Create a new directional light.
//!
//! A maximum of 4 directional lights are supported by the renderer. If you create more than that, the 4 with the highest
//! priority will be used. See @ref sgd_SetLightPriority.
SGD_API SGD_Light SGD_DECL sgd_CreateDirectionalLight();

//! Create a new point light.
//!
//! A maximum of 32 point lights are supported by the renderer. If you create more than that, the 32 with the highest priority
//! will be used. See @ref sgd_SetLightPriority. If there are more than 32 with the highest priority, the 32 nearest the camera
//! will be used.
SGD_API SGD_Light SGD_DECL sgd_CreatePointLight();

//! Create a new spot light.
//!
//! A maximum of 16 spot lights are supported by the renderer. If you create more than that, the 16 with the highest priority
//! will be used. See @ref sgd_SetLightPriority. If there are more than 16 with the highest priority, the 16 nearest the camera
//! will be used.
SGD_API SGD_Light SGD_DECL sgd_CreateSpotLight();

//! Get Light type.
SGD_API SGD_LightType SGD_DECL sgd_GetLightType(SGD_Light light);

//! Set light shadow mapping enabled, defaults to false.
SGD_API void SGD_DECL sgd_SetLightShadowsEnabled(SGD_Light light, SGD_Bool enabled);

//! Get light shadow mapping enabled.
SGD_API SGD_Bool SGD_DECL sgd_IsLightShadowsEnabled(SGD_Light light);

//! Set light priority, defaults to 0.
SGD_API void SGD_DECL sgd_SetLightPriority(SGD_Light light, int priority);

//! Get light priority.
SGD_API int SGD_DECL sgd_GetLightPriority(SGD_Light light);

//! Set light color.
SGD_API void SGD_DECL sgd_SetLightColor(SGD_Light light, float red, float green, float blue, float alpha);

//! Set point/spot light range.
SGD_API void SGD_DECL sgd_SetLightRange(SGD_Light light, float range);

//! Set point/spot light falloff.
SGD_API void SGD_DECL sgd_SetLightFalloff(SGD_Light light, float falloff);

//! Set spot light inner cone angle in degrees.
SGD_API void SGD_DECL sgd_SetLightInnerConeAngle(SGD_Light light, float angle);

//! Set spot light outer cone angle in degrees.
SGD_API void SGD_DECL sgd_SetLightOuterConeAngle(SGD_Light light, float angle);

//! @}

//! @defgroup ModelTypes ModelTypes
//! @{

//! Model handle type.
typedef SGD_Entity SGD_Model;

//! Animation modes.
typedef enum SGD_AnimationMode {
	SGD_ANIMATION_MODE_ONE_SHOT = 1,
	SGD_ANIMATION_MODE_LOOP = 2,
	SGD_ANIMATION_MODE_PING_PONG = 3
} SGD_AnimationMode;

//! @}

//! @defgroup Model Model
//! @{

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
SGD_API void SGD_DECL sgd_AnimateModel(SGD_Model model, int animation, float time, SGD_AnimationMode animationMode,
									   float weight);

//! @}

//! @defgroup SkyboxTypes SkyboxTypes
//! @{

//! Skybox handle type.
typedef SGD_Entity SGD_Skybox;

//! @}

//! @defgroup Skybox Skybox
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

//! @defgroup TerrainTypes TerrainTypes
//! @{

//! Terrain handle type.
typedef SGD_Entity SGD_Terrain;

//! @}

//! @defgroup Terrain Terrain
//! @{

//! Create a new terrain.
SGD_API SGD_Terrain SGD_DECL sgd_CreateTerrain();

//! Set terrain size.
SGD_API void SGD_DECL sgd_SetTerrainSize(SGD_Terrain terrain, int size);

//! Set terrain level-of-details.
SGD_API void SGD_DECL sgd_SetTerrainLODs(SGD_Terrain terrain, int lods);

//! Set terrain material.
SGD_API void SGD_DECL sgd_SetTerrainMaterial(SGD_Terrain terrain, SGD_Material material);

//! Set terrain material size.
SGD_API void SGD_DECL sgd_SetTerrainMaterialSize(SGD_Terrain terrain, int materialSize);

//! Set terrain height texture.
SGD_API void SGD_DECL sgd_SetTerrainHeightTexture(SGD_Terrain terrain, SGD_Texture texture);

//! Set terrain normal texture.
SGD_API void SGD_DECL sgd_SetTerrainNormalTexture(SGD_Terrain terrain, SGD_Texture texture);

//! Set terrain debug mode.
SGD_API void SGD_DECL sgd_SetTerrainDebugMode(SGD_Terrain terrain, int debugMode);

//! @}

//! @defgroup SpriteTypes SpriteTypes
//! @{

//! Sprite handle type.
typedef SGD_Entity SGD_Sprite;

//! @}

//! @defgroup Sprite Sprite
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

//! @defgroup CollisionTypes CollisionTypes
//! @{

//! Collider handle type.
typedef SGD_Handle SGD_Collider;

//! Collision responses
typedef enum SGD_CollisionResponse {
	SGD_COLLISION_RESPONSE_NONE = 0,
	SGD_COLLISION_RESPONSE_STOP = 1,
	SGD_COLLISION_RESPONSE_SLIDE = 2,
	SGD_COLLISION_RESPONSE_SLIDEXZ = 3
} SGD_CollisionResponse;

//! @}

//! @defgroup Collision Collision
//! @{

//! Create a new sphere collider and attach it to entity.
SGD_API SGD_Collider SGD_DECL sgd_CreateSphereCollider(SGD_Entity entity, int colliderType, float radius);

//! Create a new ellipsoid collider and attach it to entity.
SGD_API SGD_Collider SGD_DECL sgd_CreateEllipsoidCollider(SGD_Entity entity, int colliderType, float radius, float height);

//! Create a new mesh collider and attach it to entity.
SGD_API SGD_Collider SGD_DECL sgd_CreateMeshCollider(SGD_Entity entity, int colliderType, SGD_Mesh mesh);

//! Return entity a collider is attached to.
SGD_API SGD_Entity SGD_DECL sgd_GetColliderEntity(SGD_Collider collider);

//! Get collider type.
SGD_API int SGD_DECL sgd_GetColliderType(SGD_Collider collider);

//! Set sphere or ellipsoid collider radius.
SGD_API void SGD_DECL sgd_SetColliderRadius(SGD_Collider collider, float radius);

//! Set ellipsoid collider height.
SGD_API void SGD_DECL sgd_SetColliderHeight(SGD_Collider collider, float height);

//! Enable collisons between 2 collider types.
SGD_API void SGD_DECL sgd_EnableCollisions(int srcColliderType, int dstColliderType, SGD_CollisionResponse response);

//! Update enabled colliders.
SGD_API void SGD_DECL sgd_UpdateColliders();

//! Get collisions generated by sgd_UpdateColliders for moving collider.
SGD_API int SGD_DECL sgd_GetCollisionCount(SGD_Collider collider);

//! Get the collider a collision was with.
SGD_API SGD_Collider SGD_DECL sgd_GetCollisionCollider(SGD_Collider collider, int index);

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

//! @defgroup Picking Picking
//! @{

//! Pick first collider along ray passing from camera eye through window coordinates.
//!
//! Note that the colliderMask parameter is a 'bitmask' value, where each '1' bit in the value's binary representation
//! repesents a collider type you want to be included in the pick, allowing you to pick more than 1 collider type in a single
//! pick.
//!
//! For example, if you want the pick to include collider types 0 and 3, you would set bits 0 and 3 in the colliderMask,
//! resuling in a value of 2^0 | 2^3 == 9.
//!
//! If you want the pick to include *all* collider types, use a colliderMask of -1, a bitmask value with all bits set.
SGD_API SGD_Collider SGD_DECL sgd_CameraPick(SGD_Camera camera, float windowX, float windowY, int colliderMask);

//! Pick first collider along line.
//!
//! Please see @ref sgd_CameraPick for more information about the colliderMask parameter.
SGD_API SGD_Collider SGD_DECL sgd_LinePick(SGD_Real x0, SGD_Real y0, SGD_Real z0, SGD_Real x1, SGD_Real y1, SGD_Real z1,
										   SGD_Real radius, int colliderMask);

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

//! @defgroup RenderEffectTypes RenderEffectTypes
//! @{

//! Render effect handle type.
typedef SGD_Handle SGD_RenderEffect;

//! @}

//! @defgroup RenderEffect RenderEffect
//! @{

//! Create a new bloom effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateBloomEffect();

// Set bloom render effect radius, should be in the range 1 to 31 inclusive, defaults to 2.
SGD_API void SGD_DECL sgd_SetBloomEffectRadius(SGD_RenderEffect effect, int radius);

// Get bloom render effect radius.
SGD_API int SGD_DECL sgd_GetBloomEffectRadius(SGD_RenderEffect effect);

//! Create a new blur render effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateBlurEffect();

// Set blur render effect radius, should be in the range 1 to 31 inclusive, defaults to 2.
SGD_API void SGD_DECL sgd_SetBlurEffectRadius(SGD_RenderEffect effect, int radius);

// Get blur render effect radius.
SGD_API int SGD_DECL sgd_GetBlurEffectRadius(SGD_RenderEffect effect);

//! Create a new fog render effect and add it to the scene.
SGD_API SGD_RenderEffect SGD_DECL sgd_CreateFogEffect();

// Set fog render effect color. Defaults to .8, .9, 1, 1
SGD_API void SGD_DECL sgd_SetFogEffectColor(SGD_RenderEffect effect, float red, float green, float blue, float alpha);

// Set fog render effect range. Defaults to .1, 1000
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

//! @defgroup Audio Audio
//! @{

//! Sound handle type.
typedef SGD_Handle SGD_Sound;

//! Audo listener handle type.
typedef SGD_Handle SGD_AudioListener;

//! @}

//! @defgroup Audio Audio
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

//! Set 3D audio config. All paremters default to 1.
SGD_API void SGD_DECL sgd_Set3DAudioConfig(float rolloffScale, float dopplerScale, float distanceScale);

//! Create 3D audio listener attached to entity. Only one audio listener is currently supported.
SGD_API SGD_AudioListener SGD_DECL sgd_Create3DAudioListener(SGD_Entity entity);

//! Play 3D sound at entity position and velocity.
SGD_API int SGD_DECL sgd_Play3DSound(SGD_Sound sound, SGD_Entity entity);

//! Update 3D audio. Must be called to update 3D rolloff and doppler effects.
SGD_API void SGD_DECL sgd_Update3DAudio();

//! @}

//! @defgroup ImGui ImGui
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
