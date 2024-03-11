#include "internal.h"

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

SGD_Texture SGD_DECL sgd_LoadTexture(SGD_String path, int format, int flags) {
	auto texture = sgd::loadTexture(sgd::Path(path), (sgd::TextureFormat)format, (sgd::TextureFlags)flags);
	if (!texture) sgd::error("Failed to load texture", texture.error());

	return sgd::createHandle(texture.result());
}

// ***** Material *****

SGD_Material SGD_DECL sgd_LoadMaterial(SGD_String path) {
	auto material = sgd::loadMaterial(sgd::Path(path));
	if (!material) sgd::error("Failed to load material", material.error());

	return sgd::createHandle(material.result());
}

// ***** Mesh *****

SGD_Mesh SGD_DECL sgd_LoadMesh(SGD_String path) {
	auto mesh = sgd::loadStaticMesh(sgd::Path(path));
	if (!mesh) sgd::error("Failed to load mesh", mesh.error());

	return sgd::createHandle(mesh.result());
}

SGD_API void SGD_DECL sgd_FitMesh(SGD_Mesh hmesh, float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
								  int uniform) {
	auto mesh = sgd::resolveHandle<sgd::Mesh>(hmesh);

	fit(mesh, {{minX, minY, minZ}, {maxX, maxY, maxZ}}, uniform);
}

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

SGD_Skybox SGD_DECL sgd_LoadSkybox(SGD_String path, float roughness) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto texture = sgd::loadTexture(sgd::Path(path), sgd::TextureFormat::srgba8,
									sgd::TextureFlags::cube | sgd::TextureFlags::mipmap | sgd::TextureFlags::filter);
	if (!texture) sgd::error("Failed to load texture", texture.error());

	auto skybox = new sgd::Skybox();
	skybox->skyTexture = texture.result();
	skybox->roughness = roughness;

	sgd::mainScene->add(skybox);

	return sgd::createHandle(skybox);
}

SGD_Skybox SGD_DECL sgd_CreateSkybox() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto skybox = new sgd::Skybox();

	sgd::mainScene->add(skybox);

	return sgd::createHandle(skybox);
}

void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox hskybox, SGD_Texture htexture) {
	auto skybox = sgd::resolveHandle<sgd::Skybox>(hskybox);
	auto texture = sgd::resolveHandle<sgd::Texture>(htexture);

	skybox->skyTexture = texture;
}

SGD_API void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox hskybox, float roughness) {
	auto skybox = sgd::resolveHandle<sgd::Skybox>(hskybox);
	if (roughness < -1 || roughness > 1) sgd::error("Skybox roughness outside of range -1 to 1");

	skybox->roughness = roughness;
}

// ***** Model *****

SGD_Model SGD_DECL sgd_LoadModel(SGD_String path) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto mesh = sgd::loadStaticMesh(sgd::Path(path));
	if (!mesh) sgd::error("Failed to load mesh", mesh.error());

	auto model = new sgd::Model();
	model->mesh = mesh.result();

	sgd::mainScene->add(model);

	return sgd::createHandle(model);
}

SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path, SGD_Bool skinned) {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	sgd::Model* model;

	if (skinned) {
		auto r = sgd::loadSkinnedModel(sgd::Path(path));
		if (!r) sgd::error("Failed to load model", r.error());
		model = r.result();
	} else {
		auto r = sgd::loadBonedModel(sgd::Path(path));
		if (!r) sgd::error("Failed to load model", r.error());
		model = r.result();
	}

	sgd::mainScene->add(model);

	return sgd::createHandle(model);
}

void SGD_DECL sgd_AnimateModel(SGD_Model hmodel, int animation, float time, int mode) {
	auto model = sgd::resolveHandle<sgd::Model>(hmodel);

	model->animate(animation, time, sgd::AnimationMode(mode));
}

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

// ***** Camera *****

SGD_Camera SGD_DECL sgd_CreatePerspectiveCamera() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto camera = new sgd::Camera(sgd::CameraType::perspective);

	sgd::mainScene->add(camera);

	return createHandle(camera);
}

SGD_Camera SGD_DECL sgd_CreateOrthographicCamera() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto camera = new sgd::Camera(sgd::CameraType::orthographic);

	sgd::mainScene->add(camera);

	return createHandle(camera);
}

void SGD_DECL sgd_SetCameraFovY(SGD_Camera hcamera, float fovY) {
	auto camera = sgd::resolveHandle<sgd::Camera>(hcamera);

	camera->fovY = fovY;
}

void SGD_DECL sgd_SetCameraNear(SGD_Camera hcamera, float near) {
	auto camera = sgd::resolveHandle<sgd::Camera>(hcamera);

	camera->near = near;
}

void SGD_DECL sgd_SetCameraFar(SGD_Camera hcamera, float far) {
	auto camera = sgd::resolveHandle<sgd::Camera>(hcamera);

	camera->far = far;
}

// ***** Light *****

SGD_Light SGD_DECL sgd_CreateDirectionalLight() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto light = new sgd::Light(sgd::LightType::directional);

	sgd::mainScene->add(light);

	return sgd::createHandle(light);
}

SGD_Light SGD_DECL sgd_CreatePointLight() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto light = new sgd::Light(sgd::LightType::point);

	sgd::mainScene->add(light);

	return sgd::createHandle(light);
}

SGD_Light SGD_DECL sgd_CreateSpotLight() {
	if (!sgd::mainScene) sgd::error("Scene has not been created");

	auto light = new sgd::Light(sgd::LightType::spot);

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

void SGD_DECL sgd_SetLightInnerConeAngle(SGD_Light hlight, float angle) {
	auto light = sgd::resolveHandle<sgd::Light>(hlight);

	light->innerConeAngle = angle;
}

void SGD_DECL sgd_SetLightOuterConeAngle(SGD_Light hlight, float angle) {
	auto light = sgd::resolveHandle<sgd::Light>(hlight);

	light->outerConeAngle = angle;
}

// ***** Entity *****

SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	auto copy = entity->copy();
	sgd::mainScene->add(copy);

	return createHandle(copy);
}

void SGD_DECL sgd_SetEntityParent(SGD_Entity hentity, SGD_Entity hparent) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);
	auto parent = hparent ? sgd::resolveHandle<sgd::Entity>(hentity) : nullptr;

	entity->setParent(parent);
}

SGD_Entity SGD_DECL sgd_EntityParent(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	auto parent = entity->parent();

	return parent ? sgd::getOrCreateHandle(parent) : 0;
}

void SGD_DECL sgd_SetEntityPosition(SGD_Entity hentity, float tx, float ty, float tz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	setPosition(entity, {tx, ty, tz});
}

void SGD_DECL sgd_SetEntityRotation(SGD_Entity hentity, float rx, float ry, float rz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	setRotation(entity, {rx * sgd::degreesToRadians, ry * sgd::degreesToRadians, rz * sgd::degreesToRadians});
}

void SGD_DECL sgd_MoveEntity(SGD_Entity hentity, float tx, float ty, float tz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	move(entity, {tx, ty, tz});
}

void SGD_DECL sgd_TurnEntity(SGD_Entity hentity, float rx, float ry, float rz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	turn(entity, {rx * sgd::degreesToRadians, ry * sgd::degreesToRadians, rz * sgd::degreesToRadians});
}

void SGD_DECL sgd_TranslateEntity(SGD_Entity hentity, float tx, float ty, float tz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	translate(entity, {tx, ty, tz});
}

void SGD_DECL sgd_RotateEntity(SGD_Entity hentity, float rx, float ry, float rz) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	rotate(entity, {rx * sgd::degreesToRadians, ry * sgd::degreesToRadians, rz * sgd::degreesToRadians});
}

float SGD_DECL sgd_EntityX(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	return entity->worldMatrix().t.x;
}

float SGD_DECL sgd_EntityY(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	return entity->worldMatrix().t.y;
}

float SGD_DECL sgd_EntityZ(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	return entity->worldMatrix().t.z;
}

float SGD_DECL sgd_EntityRX(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	return sgd::pitch(entity->worldMatrix().r);
}

float SGD_DECL sgd_EntityRY(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	return sgd::yaw(entity->worldMatrix().r);
}

float SGD_DECL sgd_EntityRZ(SGD_Entity hentity) {
	auto entity = sgd::resolveHandle<sgd::Entity>(hentity);

	return sgd::roll(entity->worldMatrix().r);
}
