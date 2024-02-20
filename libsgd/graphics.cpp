#include "graphics.h"

#include "internal.h"

#include <graphics/exports.h>

void sgd_CreateScene() {
	auto gc = new sgd::GraphicsContext(sgd::mainWindow);

	sgd::mainScene = new sgd::Scene(gc);
}

void sgd_SetSceneAmbientLightColor(float red, float green, float blue, float alpha) {
	sgd::mainScene->ambientLightColor = sgd::Vec4f(red, green, blue, alpha);
}

void sgd_SetSceneClearColor(float red, float green, float blue, float alpha) {
	sgd::mainScene->clearColor = sgd::Vec4f(red, green, blue, alpha);
}

void sgd_SetSceneClearDepth(float depth) {
	sgd::mainScene->clearDepth = depth;
}

SGD_Material SGD_LoadMaterial(SGD_String path){
	auto material = sgd::loadMaterial(sgd::Path(path)).result();

	return sgd::createHandle(material);
}

SGD_Mesh sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, SGD_Material hmaterial) {
	auto material = sgd::resolveHandle<sgd::Material>(hmaterial);

	auto mesh = sgd::createBoxMesh(sgd::Boxf({minX,minY,minZ},{maxX,maxY,maxZ}), material);

	return sgd::createHandle(mesh);
}

SGD_Mesh sgd_CreateSphereMesh(float radius, int xSegs, int ySegs, SGD_Material hmaterial){
	auto material = sgd::resolveHandle<sgd::Material>(hmaterial);

	auto mesh = sgd::createSphereMesh(radius, xSegs, ySegs, material);

	return sgd::createHandle(mesh);
}

SGD_Model sgd_CreateModel(SGD_Mesh hmesh) {
	auto mesh = sgd::resolveHandle<sgd::Mesh>(hmesh);

	auto model = new sgd::Model(mesh);

	return sgd::createHandle(model);
}
