#include "internal.h"

#include <sgd/graphics.h>

#include <graphics/exports.h>

// ***** Scene *****

void SGD_DECL sgd_CreateScene() {
	if (sgd::mainScene) sgd::error("Scene has already been created");

	sgd::mainGC = new sgd::GraphicsContext(sgd::mainWindow);

	sgd::mainScene = new sgd::Scene(sgd::mainGC);
}

void SGD_DECL sgd_SetSceneAmbientLightColor(float red, float green, float blue, float alpha) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	sgd::mainScene->ambientLightColor = sgd::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetSceneClearColor(float red, float green, float blue, float alpha) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	sgd::mainScene->clearColor = sgd::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetSceneClearDepth(float depth) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	sgd::mainScene->clearDepth = depth;
}

void SGD_DECL sgd_SetSceneEnvTexture(SGD_Texture htexture) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");
	auto texture = sgd::resolveHandle<sgd::Texture>(htexture);

	sgd::mainScene->envTexture = texture;
}

void SGD_DECL sgd_RenderScene() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	sgd::mainScene->render();
}

void SGD_DECL sgd_Present() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	sgd::mainGC->present(sgd::mainGC->colorBuffer());
}

// ***** Texture *****

SGD_EXTERN SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags) {
	auto texture = sgd::loadTexture(sgd::Path(path), (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	if (!texture) sgd::error(sgd::String("sgd_LoadTexture failed: ") + texture.error().message());

	return sgd::createHandle(texture.result());
}

// ***** Material *****

SGD_Material SGD_DECL sgd_LoadMaterial(SGD_String path) {
	auto material = sgd::loadMaterial(sgd::Path(path));
	if (!material) sgd::error("Failed to load material");

	return sgd::createHandle(material.result());
}

// ***** Mesh *****

SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
									SGD_Material hmaterial) {
	auto material = sgd::resolveHandle<sgd::Material>(hmaterial);

	auto mesh = sgd::createBoxMesh(sgd::Boxf({minX, minY, minZ}, {maxX, maxY, maxZ}), material);

	return sgd::createHandle(mesh);
}

SGD_Mesh SGD_DECL sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material hmaterial) {
	auto material = sgd::resolveHandle<sgd::Material>(hmaterial);

	auto mesh = sgd::createSphereMesh(radius, xSegs, ySegs, material);

	return sgd::createHandle(mesh);
}

void SGD_DECL sgd_TransformMeshTexCoords(SGD_Mesh hmesh, float scaleX, float scaleY, float offsetX, float offsetY) {
	auto mesh = sgd::resolveHandle<sgd::Mesh>(hmesh);

	sgd::transformTexCoords(mesh, sgd::Vec2f(scaleX, scaleY), sgd::Vec2f(offsetX, offsetY));
}

// ***** Skybox *****

SGD_EXTERN SGD_Skybox SGD_DECL sgd_CreateSkybox() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto skybox = new sgd::Skybox();

	sgd::mainScene->add(skybox);

	return sgd::createHandle(skybox);
}

SGD_EXTERN void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox hskybox, SGD_Texture htexture) {
	auto skybox = sgd::resolveHandle<sgd::Skybox>(hskybox);
	auto texture = sgd::resolveHandle<sgd::Texture>(htexture);

	skybox->skyTexture = texture;
}

// ***** Model *****

SGD_Model SGD_DECL sgd_CreateModel() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto model = new sgd::Model();

	sgd::mainScene->add(model);

	return sgd::createHandle(model);
}

void SGD_DECL sgd_SetModelMesh(SGD_Model hmodel, SGD_Mesh hmesh) {
	auto model = sgd::resolveHandle<sgd::Model>(hmodel);
	auto mesh = sgd::resolveHandle<sgd::Mesh>(hmesh);

	model->mesh = mesh;
}

void SGD_DECL sgd_SetModelColor(SGD_Model hmodel, float red, float green, float blue, float alpha) {
	auto model = sgd::resolveHandle<sgd::Model>(hmodel);

	model->color = sgd::Vec4f(red, green, blue, alpha);
}

// ***** Light *****

SGD_Light SGD_DECL sgd_CreateLight() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto light = new sgd::Light();

	sgd::mainScene->add(light);

	return sgd::createHandle(light);
}

void SGD_DECL sgd_SetLightColor(SGD_Light hlight, float red, float green, float blue, float alpha) {
	auto light = sgd::resolveHandle<sgd::Light>(hlight);

	light->color = sgd::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetLightRange(SGD_Light hlight, float range) {
	auto light = sgd::resolveHandle<sgd::Light>(hlight);

	light->range = range;
}

void SGD_DECL sgd_SetLightFalloff(SGD_Light hlight, float falloff) {
	auto light = sgd::resolveHandle<sgd::Light>(hlight);

	light->falloff = falloff;
}

// ***** Entity *****

void SGD_DECL sgd_MoveEntity(SGD_Entity hentity, float tx, float ty, float tz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	move(entity, {tx, ty, tz});
}

void SGD_DECL sgd_TurnEntity(SGD_Entity hentity, float rx, float ry, float rz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	turn(entity, {rx, ry, rz});
}
