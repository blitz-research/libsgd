#include "internal.h"

// ***** Scene *****

namespace {

wgpu::BackendType g_backendType{wgpu::BackendType::Undefined};

void createOverlay() {
	if (!sgdx::g_defaultFont) {
#if _WIN32
		auto font = sgd::loadFont(sgd::Path("C:/windows/fonts/consola.ttf"), 16);
		if (font) sgdx::g_defaultFont = font.result();
#endif
	}
	sgdx::g_overlay = new sgd::Overlay();
	sgdx::g_mainScene->add(sgdx::g_overlay);
	sgdx::g_drawList = sgdx::g_overlay->drawList();
	sgdx::g_drawList->font = sgdx::g_defaultFont;
}

}

SGD_API void SGD_DECL sgd_SetWebGPUBackend(SGD_String backend) {
	if (sgdx::g_mainScene) sgdx::error("Backend type must be selected before scene is created");

	sgdx::Map<sgdx::String, wgpu::BackendType> types{
		{"D3D12", wgpu::BackendType::D3D12},   {"D3D11", wgpu::BackendType::D3D11},		  {"Vulkan", wgpu::BackendType::Vulkan},
		{"OpenGL", wgpu::BackendType::OpenGL}, {"OpenGLES", wgpu::BackendType::OpenGLES}, {"Metal", wgpu::BackendType::Metal},
		{"WebGPU", wgpu::BackendType::WebGPU},
	};
	auto it = types.find(backend);
	if (it == types.end()) sgdx::error(sgdx::String("Unrecognized backend: '") + backend + "'");

	g_backendType = it->second;
}

void SGD_DECL sgd_CreateScene() {
	if (sgdx::g_mainScene) sgdx::error("Scene has already been created");
	sgdx::mainWindow();

	sgdx::g_mainGC = new sgdx::GraphicsContext(sgdx::g_mainWindow, g_backendType);

	sgdx::g_mainScene = new sgdx::Scene(sgdx::g_mainGC);

	createOverlay();
}

void SGD_DECL sgd_ClearScene() {
	sgdx::mainScene()->clear();

	createOverlay();

	sgdx::destroyAllHandles();
}

void SGD_DECL sgd_SetSceneAmbientLightColor(float red, float green, float blue, float alpha) {
	sgdx::mainScene()->ambientLightColor = sgdx::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetSceneClearColor(float red, float green, float blue, float alpha) {
	sgdx::mainScene()->clearColor = sgdx::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetSceneClearDepth(float depth) {
	sgdx::mainScene()->clearDepth = depth;
}

void SGD_DECL sgd_SetSceneEnvTexture(SGD_Texture htexture) {
	auto texture = sgdx::resolveHandle<sgdx::Texture>(htexture);

	sgdx::mainScene()->envTexture = texture;
}

void SGD_DECL sgd_RenderScene() {
	sgdx::mainScene()->render();
}

void SGD_DECL sgd_Present() {
	sgdx::mainGC()->present(sgdx::mainGC()->colorBuffer());
}

float SGD_DECL sgd_FPS() {
	return sgdx::mainGC()->FPS();
}

// ***** Entity *****

void SGD_DECL sgd_DestroyEntity(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	sgd::Function<void(sgd::Entity*)> destroyHandles;

	destroyHandles = [&](sgd::Entity* entity) {
		for (sgd::Entity* child : entity->children()) {
			destroyHandles(child);
		}
		if (!sgdx::destroyHandle(entity)) {} // sgd::log() << "!!! SGDX Failed to destroy handle for entity:" << entity;
	};

	destroyHandles(entity);

	sgdx::mainScene()->remove(entity);
}

SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	auto copy = entity->copy();

	sgdx::mainScene()->add(copy);

	return sgdx::createHandle(copy);
}

void SGD_DECL sgd_SetEntityParent(SGD_Entity hentity, SGD_Entity hparent) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto parent = hparent ? sgdx::resolveHandle<sgd::Entity>(hparent) : nullptr;

	entity->setParent(parent);
}

SGD_Entity SGD_DECL sgd_EntityParent(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto parent = entity->parent();

	return parent ? sgdx::getOrCreateHandle(parent) : 0;
}

void SGD_DECL sgd_SetEntityPosition(SGD_Entity hentity, SGD_Real tx, SGD_Real ty, SGD_Real tz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	setPosition(entity, {tx, ty, tz});
}

void SGD_DECL sgd_SetEntityRotation(SGD_Entity hentity, SGD_Real rx, SGD_Real ry, SGD_Real rz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	setRotation(entity, {rx, ry, rz});
}

void SGD_DECL sgd_SetEntityScale(SGD_Entity hentity, SGD_Real sx, SGD_Real sy, SGD_Real sz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	setScale(entity, {sx, sy, sz});
}

void SGD_DECL sgd_MoveEntity(SGD_Entity hentity, SGD_Real tx, SGD_Real ty, SGD_Real tz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	move(entity, {tx, ty, tz});
}

void SGD_DECL sgd_TurnEntity(SGD_Entity hentity, SGD_Real rx, SGD_Real ry, SGD_Real rz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	turn(entity, {rx, ry, rz});
}

void SGD_DECL sgd_TranslateEntity(SGD_Entity hentity, SGD_Real tx, SGD_Real ty, SGD_Real tz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	translate(entity, {tx, ty, tz});
}

void SGD_DECL sgd_RotateEntity(SGD_Entity hentity, SGD_Real rx, SGD_Real ry, SGD_Real rz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	rotate(entity, {rx, ry, rz});
}

void SGD_DECL sgd_ScaleEntity(SGD_Entity hentity, SGD_Real sx, SGD_Real sy, SGD_Real sz) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	scale(entity, {sx, sy, sz});
}

SGD_Real SGD_DECL sgd_EntityX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return entity->worldMatrix().t.x;
}

SGD_Real SGD_DECL sgd_EntityY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return entity->worldMatrix().t.y;
}

SGD_Real SGD_DECL sgd_EntityZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return entity->worldMatrix().t.z;
}

SGD_Real SGD_DECL sgd_EntityRX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return sgdx::pitch(entity->worldMatrix().r) * sgdx::radiansToDegrees;
}

SGD_Real SGD_DECL sgd_EntityRY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return sgdx::yaw(entity->worldMatrix().r) * sgdx::radiansToDegrees;
}

SGD_Real SGD_DECL sgd_EntityRZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return sgdx::roll(entity->worldMatrix().r) * sgdx::radiansToDegrees;
}

SGD_Real SGD_DECL sgd_EntitySX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return entity->worldScale().x;
}

SGD_Real SGD_DECL sgd_EntitySY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return entity->worldScale().y;
}

SGD_Real SGD_DECL sgd_EntitySZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);

	return entity->worldScale().z;
}

// ***** Camera *****

SGD_Camera SGD_DECL sgd_CreatePerspectiveCamera() {

	auto camera = new sgdx::Camera(sgdx::CameraType::perspective);

	sgdx::mainScene()->add(camera);

	return sgdx::createHandle(camera);
}

SGD_Camera SGD_DECL sgd_CreateOrthographicCamera() {
	auto camera = new sgdx::Camera(sgdx::CameraType::orthographic);

	sgdx::mainScene()->add(camera);

	return sgdx::createHandle(camera);
}

void SGD_DECL sgd_SetCameraFOV(SGD_Camera hcamera, float fov) {
	auto camera = sgdx::resolveHandle<sgdx::Camera>(hcamera);

	camera->fov = fov;
}

void SGD_DECL sgd_SetCameraNear(SGD_Camera hcamera, float near) {
	auto camera = sgdx::resolveHandle<sgdx::Camera>(hcamera);

	camera->near = near;
}

void SGD_DECL sgd_SetCameraFar(SGD_Camera hcamera, float far) {
	auto camera = sgdx::resolveHandle<sgdx::Camera>(hcamera);

	camera->far = far;
}

// ***** Light *****

SGD_Light SGD_DECL sgd_CreateDirectionalLight() {
	auto light = new sgdx::Light(sgdx::LightType::directional);

	sgdx::mainScene()->add(light);

	return sgdx::createHandle(light);
}

SGD_Light SGD_DECL sgd_CreatePointLight() {
	auto light = new sgdx::Light(sgdx::LightType::point);

	sgdx::mainScene()->add(light);

	return sgdx::createHandle(light);
}

SGD_Light SGD_DECL sgd_CreateSpotLight() {
	auto light = new sgdx::Light(sgdx::LightType::spot);

	sgdx::mainScene()->add(light);

	return sgdx::createHandle(light);
}

void SGD_DECL sgd_SetLightColor(SGD_Light hlight, float red, float green, float blue, float alpha) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	light->color = sgdx::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetLightRange(SGD_Light hlight, float range) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	light->range = range;
}

void SGD_DECL sgd_SetLightFalloff(SGD_Light hlight, float falloff) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	light->falloff = falloff;
}

void SGD_DECL sgd_SetLightInnerConeAngle(SGD_Light hlight, float angle) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	light->innerConeAngle = angle;
}

void SGD_DECL sgd_SetLightOuterConeAngle(SGD_Light hlight, float angle) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	light->outerConeAngle = angle;
}

void SGD_DECL sgd_SetLightCastsShadow(SGD_Light hlight, SGD_Bool castsShadow) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	light->castsShadow = castsShadow;
}

SGD_Bool SGD_DECL sgd_LightCastsShadow(SGD_Light hlight) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);

	return light->castsShadow();
}

// ***** Model *****

SGD_Model SGD_DECL sgd_LoadModel(SGD_String path) {

	auto mesh = sgdx::loadStaticMesh(sgdx::Path(path));
	if (!mesh) sgdx::error("Failed to load mesh", mesh.error());

	auto model = new sgdx::Model();
	model->mesh = mesh.result();

	sgdx::mainScene()->add(model);

	return sgdx::createHandle(model);
}

SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path, SGD_Bool skinned) {

	sgdx::Model* model;

	if (skinned) {
		auto r = sgdx::loadSkinnedModel(sgdx::Path(path));
		if (!r) sgdx::error("Failed to load model", r.error());
		model = r.result();
	} else {
		auto r = sgdx::loadBonedModel(sgdx::Path(path));
		if (!r) sgdx::error("Failed to load model", r.error());
		model = r.result();
	}

	sgdx::mainScene()->add(model);

	return sgdx::createHandle(model);
}

void SGD_DECL sgd_AnimateModel(SGD_Model hmodel, int animation, float time, int mode) {
	auto model = sgdx::resolveHandle<sgdx::Model>(hmodel);

	model->animate(animation, time, (sgdx::AnimationMode)mode);
}

SGD_Model SGD_DECL sgd_CreateModel() {

	auto model = new sgdx::Model();

	sgdx::mainScene()->add(model);

	return sgdx::createHandle(model);
}

void SGD_DECL sgd_SetModelMesh(SGD_Model hmodel, SGD_Mesh hmesh) {
	auto model = sgdx::resolveHandle<sgdx::Model>(hmodel);
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);

	model->mesh = mesh;
}

SGD_Mesh SGD_DECL sgd_ModelMesh(SGD_Model hmodel) {
	auto model = sgdx::resolveHandle<sgdx::Model>(hmodel);

	auto mesh = model->mesh().get();

	return sgdx::getOrCreateHandle(mesh);
}

void SGD_DECL sgd_SetModelColor(SGD_Model hmodel, float red, float green, float blue, float alpha) {
	auto model = sgdx::resolveHandle<sgdx::Model>(hmodel);

	model->color = sgdx::Vec4f(red, green, blue, alpha);
}

// ***** Sprite *****

SGD_Sprite SGD_DECL sgd_CreateSprite() {

	auto sprite = new sgdx::Sprite();

	sgdx::mainScene()->add(sprite);

	return sgdx::createHandle<sgdx::Sprite>(sprite);
}

void SGD_DECL sgd_SetSpriteMaterial(SGD_Sprite hsprite, SGD_Material hmaterial) {
	auto sprite = sgdx::resolveHandle<sgdx::Sprite>(hsprite);
	auto material = sgdx::resolveHandle<sgdx::Material>(hmaterial);

	sprite->material = material;
}

void SGD_DECL sgd_SetSpriteColor(SGD_Sprite hsprite, float red, float green, float blue, float alpha) {
	auto sprite = sgdx::resolveHandle<sgdx::Sprite>(hsprite);

	sprite->color = sgdx::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetSpriteRect(SGD_Sprite hsprite, float minX, float minY, float maxX, float maxY) {
	auto sprite = sgdx::resolveHandle<sgdx::Sprite>(hsprite);

	sprite->rect = sgdx::Rectf(minX, minY, maxX, maxY);
}

// ***** Skybox *****

SGD_Skybox SGD_DECL sgd_LoadSkybox(SGD_String path, float roughness) {

	auto texture = sgdx::loadTexture(sgdx::Path(path), sgdx::TextureFormat::srgba8,
									 sgdx::TextureFlags::cube | sgdx::TextureFlags::mipmap | sgdx::TextureFlags::filter);
	if (!texture) sgdx::error("Failed to load texture", texture.error());

	auto skybox = new sgdx::Skybox();
	skybox->skyTexture = texture.result();
	skybox->roughness = roughness;

	sgdx::mainScene()->add(skybox);

	return sgdx::createHandle(skybox);
}

SGD_Skybox SGD_DECL sgd_CreateSkybox() {

	auto skybox = new sgdx::Skybox();

	sgdx::mainScene()->add(skybox);

	return sgdx::createHandle(skybox);
}

void SGD_DECL sgd_SetSkyboxTexture(SGD_Skybox hskybox, SGD_Texture htexture) {
	auto skybox = sgdx::resolveHandle<sgdx::Skybox>(hskybox);
	auto texture = sgdx::resolveHandle<sgdx::Texture>(htexture);

	skybox->skyTexture = texture;
}

void SGD_DECL sgd_SetSkyboxRoughness(SGD_Skybox hskybox, float roughness) {
	auto skybox = sgdx::resolveHandle<sgdx::Skybox>(hskybox);
	if (roughness < -1 || roughness > 1) sgdx::error("Skybox roughness outside of range -1 to 1");

	skybox->roughness = roughness;
}
