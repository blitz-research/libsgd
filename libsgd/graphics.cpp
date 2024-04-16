#include "internal.h"

// ***** Texture *****

SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags) {
	sgdx::started();

	auto texture = sgdx::loadTexture(sgdx::Path(path), (sgdx::TextureFormat)format, (sgdx::TextureFlags)flags);
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

	auto material = sgdx::loadPBRMaterial(sgdx::Path(path));
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

	auto material = sgdx::loadPrelitMaterial(sgdx::Path(path));
	if (!material) sgdx::error("Failed to load material", material.error());

	return sgdx::createHandle(material.result());
}

void SGD_DECL sgd_SetMaterialBlendMode(SGD_Material hmaterial, int blendMode) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	material->blendMode = (sgdx::BlendMode)blendMode;
}

void SGD_DECL sgd_SetMaterialDepthFunc(SGD_Material hmaterial, int depthFunc) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	material->depthFunc = (sgdx::DepthFunc)depthFunc;
}

void SGD_DECL sgd_SetMaterialCullMode(SGD_Material hmaterial, int cullMode) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	material->cullMode = (sgdx::CullMode)cullMode;
}

void SGD_DECL sgd_SetMaterialTexture(SGD_Material hmaterial, SGD_String property, SGD_Texture htexture) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);
	auto texture = sgdx::resolveHandle<sgdx::Texture>(htexture);

	if (!material->setTexture(property, texture)) sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialVector4f(SGD_Material hmaterial, SGD_String property, float x, float y, float z, float w) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	if (!material->setVector4f(property, {x, y, z, w}))
		sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialVector3f(SGD_Material hmaterial, SGD_String property, float x, float y, float z) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	if (!material->setVector3f(property, {x, y, z}))
		sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialVector2f(SGD_Material hmaterial, SGD_String property, float x, float y) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	if (!material->setVector2f(property, {x, y})) sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

void SGD_DECL sgd_SetMaterialFloat(SGD_Material hmaterial, SGD_String property, float n) {
	sgdx::started();

	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	if (!material->setFloat(property, n)) sgdx::error(sgd::String("Material property \"") + property + "\" not found");
}

// ***** Mesh *****

SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path) {
	sgdx::started();

	auto mesh = sgdx::loadStaticMesh(sgdx::Path(path));
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

// ***** Font *****

SGD_Font SGD_DECL sgd_LoadFont(SGD_String path, float height) {
	sgdx::started();
	auto font = sgd::loadFont(sgd::Path(path), height);
	if (!font) sgdx::error("Failed to load font:" + font.error().message());

	return sgdx::createHandle(font.result());
}

float SGD_DECL sgd_FontHeight(SGD_Font hfont) {
	auto font = sgdx::resolveHandle<sgd::Font>(hfont);

	return font->height;
}

// ***** DrawList *****

void SGD_DECL sgd_SetFont(SGD_Font hfont) {
	sgdx::drawList()->font = sgdx::resolveHandle<sgd::Font>(hfont);
}

void SGD_DECL sgd_SetFillColor(float red, float green, float blue, float alpha) {
	sgdx::drawList()->fillColor = {red, green, blue, alpha};
}

void SGD_DECL sgd_SetFillMaterial(SGD_Material hmaterial) {
	sgdx::drawList()->fillMaterial = sgdx::resolveHandle<sgd::Material>(hmaterial);
}

void SGD_DECL sgd_SetOutlineColor(float red, float green, float blue, float alpha) {
	sgdx::drawList()->outlineColor = {red, green, blue, alpha};
}

void SGD_DECL sgd_SetTextColor(float red, float green, float blue, float alpha) {
	sgdx::drawList()->textColor = {red, green, blue, alpha};
}

void SGD_DECL sgd_SetLineWidth(float width) {
	sgdx::drawList()->lineWidth=width;
}

void SGD_DECL sgd_Cls() {
	sgdx::drawList()->clear();
}

void SGD_DECL sgd_DrawLine(float x0, float y0, float x1, float y1) {
	sgdx::drawList()->addLine(x0,y0,x1,y1);
}

void SGD_DECL sgd_DrawRect(float minX, float minY, float maxX, float maxY) {
	sgdx::drawList()->addRect(minX, minY, maxX, maxY);
}

void SGD_DECL sgd_DrawOval(float minX, float minY, float maxX, float maxY) {
	sgdx::drawList()->addOval(minX, minY, maxX, maxY);
}

void SGD_DECL sgd_DrawText(SGD_String text, float x, float y) {
	sgdx::drawList()->addText(text, x, y);
}

SGD_API void SGD_DECL sgd_SetOverlayRenderCallback(void(SGD_DECL*callback)(void* wgpuRenderPassEncoder, void* context)) {


}
