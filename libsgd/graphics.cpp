#include "internal.h"

// ***** Texture *****

SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags) {
	sgdx::started();
	auto texture = sgd::loadTexture(sgd::Path(path), (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	if (!texture) sgdx::error("Failed to load texture", texture.error());
	return sgdx::createHandle(texture.result());
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
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	material->blendMode = (sgd::BlendMode)blendMode;
}

void SGD_DECL sgd_SetMaterialDepthFunc(SGD_Material hmaterial, int depthFunc) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	material->depthFunc = (sgd::DepthFunc)depthFunc;
}

void SGD_DECL sgd_SetMaterialCullMode(SGD_Material hmaterial, int cullMode) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	material->cullMode = (sgd::CullMode)cullMode;
}

void SGD_DECL sgd_SetMaterialTexture(SGD_Material hmaterial, SGD_String property, SGD_Texture htexture) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto texture = sgdx::resolveHandle<sgd::Texture>(htexture);
	if (!material->setTexture(property, texture)) sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialVector4f(SGD_Material hmaterial, SGD_String property, float x, float y, float z, float w) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	if (!material->setVector4f(property, {x, y, z, w}))
		sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialVector3f(SGD_Material hmaterial, SGD_String property, float x, float y, float z) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	if (!material->setVector3f(property, {x, y, z}))
		sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialVector2f(SGD_Material hmaterial, SGD_String property, float x, float y) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	if (!material->setVector2f(property, {x, y})) sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialFloat(SGD_Material hmaterial, SGD_String property, float n) {
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	if (!material->setFloat(property, n)) sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

// ***** Mesh *****

SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path) {
	sgdx::started();
	auto mesh = sgdx::loadStaticMesh(sgd::Path(path));
	if (!mesh) sgdx::error("Failed to load mesh", mesh.error());
	return sgdx::createHandle(mesh.result());
}

SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);
	auto mesh = sgd::createSphereMesh(radius, xSegs, ySegs, material);
	return sgdx::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
									SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);
	auto mesh = sgd::createBoxMesh(sgdx::Boxf({minX, minY, minZ}, {maxX, maxY, maxZ}), material);
	return sgdx::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CreateCylinderMesh(float height, float radius, int segs, SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);
	auto mesh = sgd::createCylinderMesh(height, radius, segs, material);
	return sgdx::createHandle(mesh);
}

//! Create a new cone mesh.
SGD_Mesh SGD_DECL sgd_CreateConeMesh(float height, float radius, int segs, SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);
	auto mesh = sgd::createConeMesh(height, radius, segs, material);
	return sgdx::createHandle(mesh);
}

//! Create a new torus mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs,
											  SGD_Material hmaterial) {
	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);
	auto mesh = sgd::createTorusMesh(outerRadius, innerRadius, outerSegs, innerSegs, material);
	return sgdx::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CopyMesh(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	return sgdx::createHandle(sgd::copy(mesh));
}

void SGD_DECL sgd_SetMeshCastsShadow(SGD_Mesh hmesh, SGD_Bool castsShadow) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	mesh->castsShadow = castsShadow;
}

SGD_Bool SGD_DECL sgd_MeshCastsShadow(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	return mesh->castsShadow();
}

void SGD_DECL sgd_UpdateMeshNormals(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	updateNormals(mesh);
}

void SGD_DECL sgd_UpdateMeshTangents(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	updateTangents(mesh);
}

SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh hmesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
								  int uniform) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	fit(mesh, {{minX, minY, minZ}, {maxX, maxY, maxZ}}, uniform);
}

void SGD_DECL sgd_TransformMeshTexCoords(SGD_Mesh hmesh, float scaleX, float scaleY, float offsetX, float offsetY) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	sgdx::transformTexCoords(mesh, sgdx::Vec2f(scaleX, scaleY), sgdx::Vec2f(offsetX, offsetY));
}

void SGD_DECL sgd_FlipMesh(SGD_Mesh hmesh) {
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	sgd::flip(mesh);
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

int SGD_DECL sgd_VertexCount(SGD_Mesh hmesh) {
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
	mesh->lockVertices(vertex, 1)[0] = {{x, y, z}, {nx, ny, nz}, {s, t}};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexPosition(SGD_Mesh hmesh, int vertex, float x, float y, float z) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->lockVertices(vertex, 1)->position = {x, y, z};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexNormal(SGD_Mesh hmesh, int vertex, float x, float y, float z) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->lockVertices(vertex, 1)->normal = {x, y, z};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexTangent(SGD_Mesh hmesh, int vertex, float x, float y, float z, float w) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->lockVertices(vertex, 1)->tangent = {x, y, z, w};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexTexCoords(SGD_Mesh hmesh, int vertex, float u, float v) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->lockVertices(vertex, 1)->texCoords = {u, v, 0};
	mesh->unlockVertices();
}

void SGD_DECL sgd_SetVertexColor(SGD_Mesh hmesh, int vertex, float r, float g, float b, float a) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	mesh->lockVertices(vertex, 1)->color = {r, g, b, a};
	mesh->unlockVertices();
}

// ***** Surfaces *****

SGD_Surface SGD_DECL sgd_CreateSurface(SGD_Mesh hmesh, int triangleCount, SGD_Material hmaterial) {
	auto mesh = sgdx::resolveHandle<sgd::Mesh>(hmesh);
	auto material = sgdx::resolveHandle<sgd::Material>(hmaterial);
	auto surface = new sgd::Surface(triangleCount, material);
	mesh->addSurface(surface);
	return sgdx::createHandle(surface);
}

void SGD_DECL sgd_ResizeTriangles(SGD_Surface hsurface, int count) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	surf->resizeTriangles(count);
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
	if (triangle < 0 || triangle >= surf->triangleCount()) sgdx::error("Triangle index out of range");
	surf->lockTriangles(triangle, 1)[0] = {(uint32_t)v0, (uint32_t)v1, (uint32_t)v2};
	surf->unlockTriangles();
}

void SGD_DECL sgd_ClearTriangles(SGD_Surface hsurface) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	surf->resizeTriangles(0);
}

int SGD_DECL sgd_TriangleCount(SGD_Surface hsurface) {
	auto surf = sgdx::resolveHandle<sgd::Surface>(hsurface);
	return (int)surf->triangleCount();
}

// ***** Font *****

SGD_Font SGD_DECL sgd_LoadFont(SGD_String path, float height) {
	sgdx::started();
	auto font = sgd::loadFont(sgd::Path(path), height);
	if (!font) sgdx::error("Failed to load font:" + font.error().message());
	return sgdx::createHandle(font.result());
}

//! Get width of text.
float SGD_DECL sgd_FontTextWidth(SGD_Font hfont, SGD_String text) {
	return sgdx::resolveHandle<sgd::Font>(hfont)->textWidth(text);
}

//! get height of font.
float SGD_DECL sgd_FontHeight(SGD_Font hfont) {
	return sgdx::resolveHandle<sgd::Font>(hfont)->height;
}

// ***** Image *****

SGD_Image SGD_DECL sgd_LoadImage(SGD_String path, int frames) {
	sgdx::started();
	auto image = sgd::loadImage(sgd::Path(path), frames);
	if (!image) sgdx::error("Failed to load image:" + image.error().message());
	return sgdx::createHandle(image.result());
}

void SGD_DECL sgd_SetImageBlendMode(SGD_Image himage, int blendMode) {
	sgdx::resolveHandle<sgd::Image>(himage)->blendMode = (sgd::BlendMode)blendMode;
}

void SGD_DECL sgd_SetImageSpriteViewMode(SGD_Image himage, int spriteViewMode) {
	sgdx::resolveHandle<sgd::Image>(himage)->spriteViewMode = (sgd::SpriteViewMode)spriteViewMode;
}

void SGD_DECL sgd_SetImageSpriteRect(SGD_Image himage, float minX, float minY, float maxX, float maxY) {
	sgdx::resolveHandle<sgd::Image>(himage)->spriteRect = {minX, minY, maxX, maxY};
}

void SGD_DECL sgd_SetImageDraw2DHandle(SGD_Image himage, float x, float y) {
	sgdx::resolveHandle<sgd::Image>(himage)->drawHandle = {x, y};
}

int SGD_DECL sgd_ImageWidth(SGD_Image himage) {
	return (int)sgdx::resolveHandle<sgd::Image>(himage)->frames()->size().x;
}

int SGD_DECL sgd_ImageHeight(SGD_Image himage) {
	return (int)sgdx::resolveHandle<sgd::Image>(himage)->frames()->size().y;
}

int SGD_DECL sgd_ImageFrameCount(SGD_Image himage) {
	return (int)sgdx::resolveHandle<sgd::Image>(himage)->frames()->depth();
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
