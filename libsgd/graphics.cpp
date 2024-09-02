#include "internal.h"

// ***** Texture *****

SGD_Texture SGD_DECL sgd_Load2DTexture(SGD_String path, int format, int flags) {
	sgdx::started();
	auto texture = sgd::load2DTexture(sgd::Path(path), (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	if (!texture) sgdx::error("Failed to load 2d texture", texture.error());
	return sgdx::createHandle(texture.result());
}

SGD_Texture SGD_DECL sgd_LoadCubeTexture(SGD_String path, int format, int flags) {
	sgdx::started();
	auto texture = sgd::loadCubeTexture(sgd::Path(path), (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	if (!texture) sgdx::error("Failed to load cube texture", texture.error());
	return sgdx::createHandle(texture.result());
}

SGD_Texture SGD_DECL sgd_LoadArrayTexture(SGD_String path, int format, int flags) {
	sgdx::started();
	auto texture = sgd::loadArrayTexture(sgd::Path(path), (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	if (!texture) sgdx::error("Failed to load array texture", texture.error());
	return sgdx::createHandle(texture.result());
}

SGD_Texture SGD_DECL sgd_Create2DTexture(int width, int height, int format, int flags) {
	sgdx::started();
	auto texture = new sgd::Texture(sgd::Vec2u(width, height), 1, (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	return sgdx::createHandle(texture);
}

SGD_Texture SGD_DECL sgd_CreateCubeTexture(int size, int format, int flags) {
	sgdx::started();
	flags |= (int)sgd::TextureFlags::cube;
	auto texture = new sgd::Texture(sgd::Vec2u(size), 6, (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	return sgdx::createHandle(texture);
}

SGD_Texture SGD_DECL sgd_CreateArrayTexture(int width, int height, int depth, int format, int flags) {
	sgdx::started();
	flags |= (int)sgd::TextureFlags::array;
	auto texture = new sgd::Texture(sgd::Vec2u(width, height), depth, (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	return sgdx::createHandle(texture);
}

int SGD_DECL sgd_GetTextureWidth(SGD_Texture htexture) {
	return (int)sgdx::resolveHandle<sgd::Texture>(htexture)->size().x;
}

int SGD_DECL sgd_GetTextureHeight(SGD_Texture htexture) {
	return (int)sgdx::resolveHandle<sgd::Texture>(htexture)->size().y;
}

int SGD_DECL sgd_GetTextureDepth(SGD_Texture htexture) {
	return (int)sgdx::resolveHandle<sgd::Texture>(htexture)->depth();
}

int SGD_DECL sgd_GetTextureFormat(SGD_Texture htexture) {
	return (int)sgdx::resolveHandle<sgd::Texture>(htexture)->format();
}

int SGD_DECL sgd_GetTextureFlags(SGD_Texture htexture) {
	return (int)sgdx::resolveHandle<sgd::Texture>(htexture)->flags();
}

void SGD_DECL sgd_SetTexelSRGBA(SGD_Texture htexture, int x, int y, int rgba) {
	auto texture = sgdx::resolveHandle<sgd::Texture>(htexture);
	auto dst = sgd::texelPtr(texture->lock(), sgd::Vec2u(x, y));
	sgd::convertTexels(&rgba, sgd::TextureFormat::srgba8, dst, texture->format(), 1);
	texture->unlock();
}

int SGD_DECL sgd_GetTexelSRGBA(SGD_Texture htexture, int x, int y) {
	auto texture = sgdx::resolveHandle<sgd::Texture>(htexture);
	auto src = sgd::texelPtr(texture->lock(), sgd::Vec2u(x, y));
	uint32_t rgba;
	sgd::convertTexels(src, texture->format(), &rgba, sgd::TextureFormat::srgba8, 1);
	texture->unlock();
	return (int)rgba;
}

// ***** Material *****

SGD_Material SGD_DECL sgd_CreatePBRMaterial() {
	sgdx::started();
	auto material = sgd::createPBRMaterial();
	return sgdx::createHandle(material);
}

SGD_Material SGD_DECL sgd_LoadPBRMaterial(SGD_String path) {
	sgdx::started();
	auto material = sgd::loadPBRMaterial(sgd::Path(path));
	if (!material) sgdx::error("Failed to load material", material.error());
	return sgdx::createHandle(material.result());
}

SGD_Material SGD_DECL sgd_CreatePrelitMaterial() {
	sgdx::started();
	auto material = sgd::createPrelitMaterial();
	return sgdx::createHandle(material);
}

SGD_Material SGD_DECL sgd_LoadPrelitMaterial(SGD_String path) {
	sgdx::started();
	auto material = sgd::loadPrelitMaterial(sgd::Path(path));
	if (!material) sgdx::error("Failed to load material", material.error());
	return sgdx::createHandle(material.result());
}

void SGD_DECL sgd_SetMaterialBlendMode(SGD_Material hmaterial, int blendMode) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->blendMode = (sgd::BlendMode)blendMode;
}

void SGD_DECL sgd_SetMaterialDepthFunc(SGD_Material hmaterial, int depthFunc) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->depthFunc = (sgd::DepthFunc)depthFunc;
}

void SGD_DECL sgd_SetMaterialCullMode(SGD_Material hmaterial, int cullMode) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->cullMode = (sgd::CullMode)cullMode;
}

void SGD_DECL sgd_SetMaterialTexture(SGD_Material hmaterial, SGD_String property, SGD_Texture htexture) {
	auto texture = sgdx::resolveHandle<sgd::Texture>(htexture);
	sgdx::resolveHandle<sgd::Material>(hmaterial)->setTexture(property, texture);
}

void SGD_DECL sgd_SetMaterialVector4f(SGD_Material hmaterial, SGD_String property, float x, float y, float z, float w) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->setVector4f(property, {x, y, z, w});
}

void SGD_DECL sgd_SetMaterialVector3f(SGD_Material hmaterial, SGD_String property, float x, float y, float z) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->setVector3f(property, {x, y, z});
}

void SGD_DECL sgd_SetMaterialVector2f(SGD_Material hmaterial, SGD_String property, float x, float y) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->setVector2f(property, {x, y});
}

void SGD_DECL sgd_SetMaterialFloat(SGD_Material hmaterial, SGD_String property, float n) {
	sgdx::resolveHandle<sgd::Material>(hmaterial)->setFloat(property, n);
}

// ***** Mesh *****

SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path) {
	sgdx::started();
	auto mesh = sgd::loadStaticMesh(sgd::Path(path));
	if (!mesh) sgdx::error("Failed to load mesh", mesh.error());
	return sgdx::createHandle(mesh.result());
}

SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto mesh = sgd::createSphereMesh(radius, xSegs, ySegs, material);
	return sgdx::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
									SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto mesh = sgd::createBoxMesh(sgd::Boxf({minX, minY, minZ}, {maxX, maxY, maxZ}), material);
	return sgdx::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CreateCylinderMesh(float height, float radius, int segs, SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto mesh = sgd::createCylinderMesh(height, radius, segs, material);
	return sgdx::createHandle(mesh);
}

//! Create a new cone mesh.
SGD_Mesh SGD_DECL sgd_CreateConeMesh(float height, float radius, int segs, SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto mesh = sgd::createConeMesh(height, radius, segs, material);
	return sgdx::createHandle(mesh);
}

//! Create a new torus mesh.
SGD_Mesh SGD_DECL sgd_CreateTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs,
									  SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto mesh = sgd::createTorusMesh(outerRadius, innerRadius, outerSegs, innerSegs, material);
	return sgdx::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CopyMesh(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return sgdx::createHandle(sgd::copy(mesh));
}

void SGD_DECL sgd_SetMeshShadowsEnabled(SGD_Mesh hmesh, SGD_Bool enabled) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->shadowsEnabled = enabled;
}

SGD_Bool SGD_DECL sgd_IsMeshShadowsEnabled(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->shadowsEnabled();
}

void SGD_DECL sgd_UpdateMeshNormals(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	updateNormals(mesh);
}

void SGD_DECL sgd_UpdateMeshTangents(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	updateTangents(mesh);
}

void SGD_DECL sgd_FitMesh(SGD_Mesh hmesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ, int uniform) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	fit(mesh, {{minX, minY, minZ}, {maxX, maxY, maxZ}}, uniform);
}

void SGD_DECL sgd_TFormMesh(SGD_Mesh hmesh, float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy,
							float sz) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	sgdx::transform(mesh, sgd::AffineMat4f::TRS({tx, ty, tz}, {rx, ry, rz}, {sx, sy, sz}));
}

void SGD_DECL sgd_TFormMeshTexCoords(SGD_Mesh hmesh, float scaleX, float scaleY, float offsetX, float offsetY) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	sgdx::transformTexCoords(mesh, sgd::Vec2f(scaleX, scaleY), sgdx::Vec2f(offsetX, offsetY));
}

void SGD_DECL sgd_FlipMesh(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	sgd::flip(mesh);
}

float SGD_DECL sgd_GetMeshBoundsMinX(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->bounds().min.x;
}

float SGD_DECL sgd_GetMeshBoundsMinY(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->bounds().min.y;
}

float SGD_DECL sgd_GetMeshBoundsMinZ(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->bounds().min.z;
}

float SGD_DECL sgd_GetMeshBoundsMaxX(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->bounds().max.x;
}

float SGD_DECL sgd_GetMeshBoundsMaxY(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->bounds().max.y;
}

float SGD_DECL sgd_GetMeshBoundsMaxZ(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return mesh->bounds().max.z;
}

// ***** Mesh Creation *****

SGD_Mesh SGD_DECL sgd_CreateMesh(int vertexCount, int flags) {
	sgdx::started();
	auto mesh = new sgd::Mesh(vertexCount, (sgd::MeshFlags)flags);
	return sgdx::createHandle(mesh);
}

void SGD_DECL sgd_ResizeVertices(SGD_Mesh hmesh, int count) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->resizeVertices(count);
}

int SGD_DECL sgd_GetVertexCount(SGD_Mesh hmesh) {
	return (int)sgdx::resolveHandle<sgd::Mesh>(hmesh)->vertexCount();
}

int SGD_DECL sgd_AddVertex(SGD_Mesh hmesh, float x, float y, float z, float nx, float ny, float nz, float s, float t) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	int i = (int)mesh->vertexCount();
	mesh->resizeVertices(i + 1);
	mesh->lockVertices(i, 1)[0] = {{x, y, z}, {nx, ny, nz}, {s, t}};
	mesh->unlockVertices();
	return i;
}

void SGD_DECL sgd_SetVertex(SGD_Mesh hmesh, int vertex, float x, float y, float z, float nx, float ny, float nz, float s,
							float t) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	mesh->lockVertices(vertex, 1)[0] = {{x, y, z}, {nx, ny, nz}, {s, t}};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexPosition(SGD_Mesh hmesh, int vertex, float x, float y, float z) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	mesh->lockVertices(vertex, 1)->position = {x, y, z};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexNormal(SGD_Mesh hmesh, int vertex, float x, float y, float z) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	mesh->lockVertices(vertex, 1)->normal = {x, y, z};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexTangent(SGD_Mesh hmesh, int vertex, float x, float y, float z, float w) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	mesh->lockVertices(vertex, 1)->tangent = {x, y, z, w};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexColor(SGD_Mesh hmesh, int vertex, float r, float g, float b, float a) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	mesh->lockVertices(vertex, 1)->color = {r, g, b, a};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexTexCoord0(SGD_Mesh hmesh, int vertex, float u0, float v0) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	mesh->lockVertices(vertex, 1)->texCoords = {u0, v0, 0};
	mesh->unlockVertices();
}

//! Get vertex position x coordinate.
float SGD_DECL sgd_GetVertexX(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].position.x;
}

//! Get vertex position y coordinate.
float SGD_DECL sgd_GetVertexY(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].position.y;
}

//! Get vertex position z coordinate.
float SGD_DECL sgd_GetVertexZ(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].position.z;
}

//! Get vertex normal x component.
float SGD_DECL sgd_GetVertexNX(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].normal.x;
}

//! Get vertex normal y component.
float SGD_DECL sgd_GetVertexNY(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].normal.y;
}

//! Get vertex normal z component.
float SGD_DECL sgd_GetVertexNZ(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].normal.z;
}

//! Get vertex tangent x component.
float SGD_DECL sgd_GetVertexTX(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].tangent.x;
}

//! Get vertex tangent y component.
float SGD_DECL sgd_GetVertexTY(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].tangent.y;
}

//! Get vertex tangent z component.
float SGD_DECL sgd_GetVertexTZ(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].tangent.z;
}

//! Get vertex tangent w component.
float SGD_DECL sgd_GetVertexTW(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].tangent.w;
}

//! Get vertex color red component.
float SGD_DECL sgd_GetVertexRed(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].color.x;
}

//! Get vertex color green component.
float SGD_DECL sgd_GetVertexGreen(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].color.y;
}

//! Get vertex color blue component.
float SGD_DECL sgd_GetVertexBlue(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].color.z;
}

//! Get vertex color alpha component.
float SGD_DECL sgd_GetVertexAlpha(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].color.w;
}

//! Get vertex texture coordinate 0 u component.
float SGD_DECL sgd_GetVertexU0(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].texCoords.x;
}

//! Get vertex tangent coorindate 1 y component.
float SGD_DECL sgd_GetVertexV0(SGD_Mesh hmesh, int vertex) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)vertex >= mesh->vertexCount()) sgdx::error("Vertex index out of range");
	return mesh->vertices()[vertex].texCoords.y;
}

// ***** Surfaces *****

SGD_Surface SGD_DECL sgd_CreateSurface(SGD_Mesh hmesh, SGD_Material hmaterial, int triangleCount) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto surface = new sgd::Surface(mesh, material, triangleCount);
	mesh->addSurface(surface);
	return sgdx::createHandle(surface);
}

int SGD_DECL sgd_GetSurfaceCount(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	return (int)mesh->surfaces().size();
}

SGD_Surface SGD_DECL sgd_GetSurface(SGD_Mesh hmesh, int surface) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	if ((uint32_t)surface >= mesh->surfaces().size()) sgdx::error("Surface index out of range");
	auto surf = mesh->surfaces()[surface];
	return sgdx::getOrCreateHandle(surf.get());
}

SGD_Material SGD_DECL sgd_GetSurfaceMaterial(SGD_Surface hsurface) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	return sgdx::getOrCreateHandle((sgd::Material*)surf->material());
}

void SGD_DECL sgd_ResizeTriangles(SGD_Surface hsurface, int count) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	surf->resizeTriangles(count);
}

int SGD_DECL sgd_GetTriangleCount(SGD_Surface hsurface) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	return (int)surf->triangleCount();
}

int SGD_DECL sgd_AddTriangle(SGD_Surface hsurface, int v0, int v1, int v2) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	int i = (int)surf->triangleCount();
	surf->resizeTriangles(i + 1);
	surf->lockTriangles(i, 1)[0] = {(uint32_t)v0, (uint32_t)v1, (uint32_t)v2};
	surf->unlockTriangles();
	return i;
}

void SGD_DECL sgd_SetTriangle(SGD_Surface hsurface, int triangle, int v0, int v1, int v2) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	if ((uint32_t)triangle >= surf->triangleCount()) sgdx::error("Triangle index out of range");
	surf->lockTriangles(triangle, 1)[0] = {(uint32_t)v0, (uint32_t)v1, (uint32_t)v2};
	surf->unlockTriangles();
}

int SGD_DECL sgd_GetTriangleVertex(SGD_Surface hsurface, int triangle, int vertex) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	if ((uint32_t)triangle >= surf->triangleCount()) sgdx::error("Triangle index out of range");
	if ((uint32_t)vertex >= 3) sgdx::error("Vertex index must be 0, 1 or 2");
	return (int)surf->triangles()[triangle].indices[vertex];
}

// ***** Font *****

SGD_Font SGD_DECL sgd_LoadFont(SGD_String path, float height) {
	sgdx::started();
	auto font = sgd::loadFont(sgd::Path(path), height);
	if (!font) sgdx::error("Failed to load font: " + font.error().message());
	return sgdx::createHandle(font.result());
}

//! Get width of text.
float SGD_DECL sgd_GetTextWidth(SGD_Font hfont, SGD_String text) {
	return sgdx::resolveHandle<sgd::Font>(hfont)->textWidth(text);
}

//! get height of font.
float SGD_DECL sgd_GetFontHeight(SGD_Font hfont) {
	return sgdx::resolveHandle<sgd::Font>(hfont)->height;
}

// ***** Image *****

SGD_Image SGD_DECL sgd_LoadImage(SGD_String path, int depth) {
	sgdx::started();
	auto image = sgd::loadImage(sgd::Path(path), depth);
	if (!image) sgdx::error("Failed to load image:" + image.error().message());
	return sgdx::createHandle(image.result());
}

SGD_Image SGD_DECL sgd_CreateImage(SGD_Texture htexture) {
	auto texture = sgdx::resolveHandle<sgd::Texture>(htexture);
	auto image = new sgd::Image(texture);
	return sgdx::createHandle(image);
}

void SGD_DECL sgd_SetImageViewMode(SGD_Image himage, int viewMode) {
	sgdx::resolveHandle<sgd::Image>(himage)->viewMode = (sgd::ImageViewMode)viewMode;
}

void SGD_DECL sgd_SetImageBlendMode(SGD_Image himage, int blendMode) {
	sgdx::resolveHandle<sgd::Image>(himage)->blendMode = (sgd::BlendMode)blendMode;
}

void SGD_DECL sgd_SetImageRect(SGD_Image himage, float minX, float minY, float maxX, float maxY) {
	sgdx::resolveHandle<sgd::Image>(himage)->rect = {minX, minY, maxX, maxY};
}

SGD_Texture SGD_DECL sgd_GetImageTexture(SGD_Image himage) {
	return sgdx::getOrCreateHandle(sgdx::resolveHandle<sgd::Image>(himage)->texture());
}

// ***** 2D Overlay *****

void SGD_DECL sgd_Set2DFillColor(float red, float green, float blue, float alpha) {
	sgdx::drawList()->fillColor = {red, green, blue, alpha};
}

void SGD_DECL sgd_Set2DFillMaterial(SGD_Material hmaterial) {
	sgdx::drawList()->fillMaterial = sgdx::resolveHandle<sgd::Material>(hmaterial);
}

void SGD_DECL sgd_Set2DFillEnabled(SGD_Bool enabled) {
	sgdx::drawList()->fillEnabled = enabled;
}

void SGD_DECL sgd_Set2DOutlineColor(float red, float green, float blue, float alpha) {
	sgdx::drawList()->outlineColor = {red, green, blue, alpha};
}

void SGD_DECL sgd_Set2DOutlineWidth(float width) {
	sgdx::drawList()->outlineWidth = width;
}

void SGD_DECL sgd_Set2DOutlineEnabled(SGD_Bool enabled) {
	sgdx::drawList()->outlineEnabled = enabled;
}

void SGD_DECL sgd_Set2DLineWidth(float width) {
	sgdx::drawList()->lineWidth = width;
}

void SGD_DECL sgd_Set2DPointSize(float size) {
	sgdx::drawList()->pointSize = size;
}

void SGD_DECL sgd_Set2DFont(SGD_Font hfont) {
	sgdx::drawList()->font = sgdx::resolveHandle<sgd::Font>(hfont);
}

void SGD_DECL sgd_Set2DTextColor(float red, float green, float blue, float alpha) {
	sgdx::drawList()->textColor = {red, green, blue, alpha};
}

float SGD_DECL sgd_Get2DTextWidth(SGD_String text) {
	return sgdx::drawList()->font()->textWidth(text);
}

float SGD_DECL sgd_Get2DFontHeight() {
	return sgdx::drawList()->font()->height;
}

void SGD_DECL sgd_Clear2D() {
	sgdx::drawList()->clear();
}

void SGD_DECL sgd_Push2DLayer() {
	sgdx::drawList()->pushLayer();
}

void SGD_DECL sgd_Pop2DLayer() {
	sgdx::drawList()->popLayer();
}

void SGD_DECL sgd_Draw2DPoint(float x, float y) {
	sgdx::drawList()->addPoint({x, y});
}

void SGD_DECL sgd_Draw2DLine(float x0, float y0, float x1, float y1) {
	sgdx::drawList()->addLine({x0, y0}, {x1, y1});
}

void SGD_DECL sgd_Draw2DRect(float minX, float minY, float maxX, float maxY) {
	sgdx::drawList()->addRect({minX, minY, maxX, maxY});
}

void SGD_DECL sgd_Draw2DOval(float minX, float minY, float maxX, float maxY) {
	sgdx::drawList()->addOval({minX, minY, maxX, maxY});
}

void SGD_DECL sgd_Draw2DImage(SGD_Image himage, float x, float y, float frame) {
	auto image = sgdx::resolveHandle<sgd::Image>(himage);
	sgdx::drawList()->addImage(image, {x, y}, frame);
}

void SGD_DECL sgd_Draw2DText(SGD_String text, float x, float y) {
	sgdx::drawList()->addText(text, {x, y});
}
