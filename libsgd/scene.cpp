#include "internal.h"

namespace {

sgd::Vec3r g_tformed;
sgd::Vec2f g_projected;
sgd::Vec3r g_unprojected;
sgd::Contact g_picked;

sgd::ConfigUniforms& lockConfigUniforms() {
	return sgdx::mainScene()->sceneRenderer()->sceneBindings()->lockConfigUniforms();
}

void unlockConfigUniforms() {
	sgdx::mainScene()->sceneRenderer()->sceneBindings()->unlockConfigUniforms();
}

} // namespace

// ***** Scene *****

// Creates a new scene if none exists.
void SGD_DECL sgd_ClearScene() {
	if (!sgdx::g_mainScene) {
		sgd::createGC(sgdx::mainWindow());
		sgdx::g_mainScene = new sgd::Scene();
	} else {
		sgdx::mainScene()->clear();
		sgdx::destroyAllHandles();
	}
	sgdx::g_overlay = new sgd::Overlay();
	sgdx::g_mainScene->add(sgdx::g_overlay);
	sgdx::g_drawList = sgdx::g_overlay->drawList();
}

void SGD_DECL sgd_SetClearColor(float red, float green, float blue, float alpha) {
	sgdx::mainScene()->sceneRenderer()->clearColor = sgdx::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetClearDepth(float depth) {
	sgdx::mainScene()->sceneRenderer()->clearDepth = depth;
}

void SGD_DECL sgd_SetAmbientLightColor(float red, float green, float blue, float alpha) {
	sgdx::mainScene()->sceneRenderer()->ambientLightColor = sgdx::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetEnvTexture(SGD_Texture htexture) {
	auto texture = sgdx::resolveHandle<sgdx::Texture>(htexture);
	sgdx::mainScene()->sceneRenderer()->envTexture = texture;
}

void SGD_DECL sgd_SetCSMTextureSize(int textureSize) {
	lockConfigUniforms().csmTextureSize = textureSize;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetMaxCSMLights(int maxLights) {
	lockConfigUniforms().maxCSMLights = maxLights;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetCSMSplitDistances(float split0, float split1, float split2, float split3) {
	lockConfigUniforms().csmSplitDistances = {split0, split1, split2, split3};
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetCSMClipRange(float range) {
	lockConfigUniforms().csmClipRange = range;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetCSMDepthBias(float bias) {
	lockConfigUniforms().csmDepthBias = bias;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetPSMTextureSize(int textureSize) {
	lockConfigUniforms().psmTextureSize = textureSize;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetMaxPSMLights(int maxLights) {
	lockConfigUniforms().maxPSMLights = maxLights;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetPSMClipNear(float near) {
	lockConfigUniforms().psmClipNear = near;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetPSMDepthBias(float bias) {
	lockConfigUniforms().psmDepthBias = bias;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetSSMTextureSize(int textureSize) {
	lockConfigUniforms().ssmTextureSize = textureSize;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetMaxSSMLights(int maxLights) {
	lockConfigUniforms().maxSSMLights = maxLights;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetSSMClipNear(float near) {
	lockConfigUniforms().ssmClipNear = near;
	unlockConfigUniforms();
}

void SGD_DECL sgd_SetSSMDepthBias(float bias) {
	lockConfigUniforms().ssmDepthBias = bias;
	unlockConfigUniforms();
}

void SGD_DECL sgd_RenderScene() {
	sgdx::mainScene()->render();
}

void SGD_DECL sgd_Present() {
	sgd::currentGC()->present(sgdx::mainScene()->sceneRenderer()->outputTexture());
}

float SGD_DECL sgd_GetFPS() {
	return sgd::currentGC()->FPS();
}

float SGD_DECL sgd_GetRPS() {
	return sgdx::mainScene()->sceneRenderer()->RPS();
}

// ***** Entity *****

void SGD_DECL sgd_SetEntityEnabled(SGD_Entity hentity, SGD_Bool enabled) {
	sgdx::resolveHandle<sgd::Entity>(hentity)->setIsEnabled(enabled);
}

SGD_Bool SGD_DECL sgd_IsEntityEnabled(SGD_Entity hentity) {
	return sgdx::resolveHandle<sgd::Entity>(hentity)->enabled();
}

void SGD_DECL sgd_SetEntityVisible(SGD_Entity hentity, SGD_Bool visible) {
	sgdx::resolveHandle<sgd::Entity>(hentity)->setIsVisible(visible);
}

SGD_Bool SGD_DECL sgd_IsEntityVisible(SGD_Entity hentity) {
	return sgdx::resolveHandle<sgd::Entity>(hentity)->visible();
}

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

void SGD_DECL sgd_ResetEntity(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	entity->reset();
}

SGD_Entity SGD_DECL sgd_CopyEntity(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto copy = entity->copy();
	sgdx::mainScene()->add(copy);
	return sgdx::createHandle(copy);
}

void SGD_DECL sgd_SetEntityName(SGD_Entity hentity, SGD_String name) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	entity->setName(name);
}

SGD_String SGD_DECL sgd_GetEntityName(SGD_Entity hentity) {
	static sgd::String name;
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	name = entity->name() + '\0';
	return name.data();
}

void SGD_DECL sgd_SetEntityParent(SGD_Entity hentity, SGD_Entity hparent) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto parent = hparent ? sgdx::resolveHandle<sgd::Entity>(hparent) : nullptr;
	entity->setParent(parent);
}

SGD_Entity SGD_DECL sgd_GetEntityParent(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto parent = entity->parent();
	return parent ? sgdx::getOrCreateHandle(parent) : SGD_NULL;
}

int SGD_DECL sgd_GetEntityChildCount(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return (int)entity->children().size();
}

SGD_Entity SGD_DECL sgd_GetEntityChild(SGD_Entity hentity, int childIndex) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	if(childIndex<0 || childIndex>=entity->children().size()) sgdx::error("Child index out of range");
	return sgdx::getOrCreateHandle(entity->children()[childIndex].get());
}

SGD_Entity SGD_DECL sgd_FindEntityChild(SGD_Entity hentity, SGD_String name) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto child = entity->findChild(name);
	return child ? sgdx::getOrCreateHandle(child) : SGD_NULL;
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

SGD_Real SGD_DECL sgd_GetEntityX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldPosition().x;
}

SGD_Real SGD_DECL sgd_GetEntityY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldPosition().y;
}

SGD_Real SGD_DECL sgd_GetEntityZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldPosition().z;
}

SGD_Real SGD_DECL sgd_GetEntityRX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return sgdx::pitch(entity->worldBasis()) * sgdx::radiansToDegrees;
}

SGD_Real SGD_DECL sgd_GetEntityRY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return sgdx::yaw(entity->worldBasis()) * sgdx::radiansToDegrees;
}

SGD_Real SGD_DECL sgd_GetEntityRZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return sgdx::roll(entity->worldBasis()) * sgdx::radiansToDegrees;
}

SGD_Real SGD_DECL sgd_GetEntitySX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldScale().x;
}

SGD_Real SGD_DECL sgd_GetEntitySY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldScale().y;
}

SGD_Real SGD_DECL sgd_GetEntitySZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldScale().z;
}

SGD_Real SGD_DECL sgd_GetEntityIX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().i.x;
}

SGD_Real SGD_DECL sgd_GetEntityIY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().i.y;
}

SGD_Real SGD_DECL sgd_GetEntityIZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().i.z;
}

SGD_Real SGD_DECL sgd_GetEntityJX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().j.x;
}

SGD_Real SGD_DECL sgd_GetEntityJY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().j.y;
}

SGD_Real SGD_DECL sgd_GetEntityJZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().j.z;
}

SGD_Real SGD_DECL sgd_GetEntityKX(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().k.x;
}

SGD_Real SGD_DECL sgd_GetEntityKY(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().k.y;
}

SGD_Real SGD_DECL sgd_GetEntityKZ(SGD_Entity hentity) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	return entity->worldBasis().k.z;
}

void SGD_DECL sgd_AimEntityAtEntity(SGD_Entity hentity, SGD_Entity htarget, float roll) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto target = sgdx::resolveHandle<sgd::Entity>(htarget);
	sgd::aim(entity, target->worldPosition(), roll);
}

void SGD_DECL sgd_AimEntityAtPoint(SGD_Entity hentity, SGD_Real x, SGD_Real y, SGD_Real z, float roll) {
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	sgd::aim(entity, {x, y, z}, roll);
}

void SGD_DECL sgd_TFormPoint(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity hsrcEntity, SGD_Entity hdstEntity) {
	auto src = sgdx::resolveHandle<sgd::Entity>(hsrcEntity);
	auto dst = sgdx::resolveHandle<sgd::Entity>(hdstEntity);
	auto tv = src ? src->worldMatrix() * sgd::Vec3r(x, y, z) : sgd::Vec3r(x, y, z);
	g_tformed = dst ? inverse(dst->worldMatrix()) * tv : tv;
}

// Includes entity scale, maybe add a bool param?
void SGD_DECL sgd_TFormVector(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity hsrcEntity, SGD_Entity hdstEntity) {
	auto src = sgdx::resolveHandle<sgd::Entity>(hsrcEntity);
	auto dst = sgdx::resolveHandle<sgd::Entity>(hdstEntity);
	auto tv = src ? src->worldMatrix().r * sgd::Vec3r(x, y, z) : sgd::Vec3r(x, y, z);
	g_tformed = dst ? inverse(dst->worldMatrix().r) * tv : tv;
}

// Does no normalization.
void SGD_DECL sgd_TFormNormal(SGD_Real x, SGD_Real y, SGD_Real z, SGD_Entity hsrcEntity, SGD_Entity hdstEntity) {
	auto src = sgdx::resolveHandle<sgd::Entity>(hsrcEntity);
	auto dst = sgdx::resolveHandle<sgd::Entity>(hdstEntity);
	auto tv = src ? cofactor(src->worldBasis()) * sgd::Vec3r(x, y, z) : sgd::Vec3r(x, y, z);
	g_tformed = dst ? cofactor(inverse(dst->worldBasis())) * tv : tv;
}

SGD_Real SGD_DECL sgd_GetTFormedX() {
	return g_tformed.x;
}

SGD_Real SGD_DECL sgd_GetTFormedY() {
	return g_tformed.y;
}

SGD_Real SGD_DECL sgd_GetTFormedZ() {
	return g_tformed.z;
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
	sgdx::resolveHandle<sgdx::Camera>(hcamera)->fov = fov;
}

void SGD_DECL sgd_SetCameraNear(SGD_Camera hcamera, float near) {
	sgdx::resolveHandle<sgdx::Camera>(hcamera)->near = near;
}

void SGD_DECL sgd_SetCameraFar(SGD_Camera hcamera, float far) {
	sgdx::resolveHandle<sgdx::Camera>(hcamera)->far = far;
}

SGD_Bool SGD_DECL sgd_CameraProject(SGD_Camera hcamera, SGD_Real x, SGD_Real y, SGD_Real z) {
	auto camera = sgdx::resolveHandle<sgdx::Camera>(hcamera);
	auto r = sgd::project(camera, {x, y, z});
	if (r) g_projected = r.result();
	return r;
}

float SGD_DECL sgd_GetProjectedX() {
	return g_projected.x;
}

float SGD_DECL sgd_GetProjectedY() {
	return g_projected.y;
}

SGD_Bool SGD_DECL sgd_CameraUnproject(SGD_Camera hcamera, float windowX, float windowY, float viewZ) {
	auto camera = sgdx::resolveHandle<sgdx::Camera>(hcamera);
	g_unprojected = unproject(camera, {windowX, windowY}, viewZ);
	return true;
}

SGD_Real SGD_DECL sgd_GetUnprojectedX() {
	return g_unprojected.x;
}

SGD_Real SGD_DECL sgd_GetUnprojectedY() {
	return g_unprojected.y;
}

SGD_Real SGD_DECL sgd_GetUnprojectedZ() {
	return g_unprojected.z;
}

// ***** Light *****

SGD_Light SGD_DECL sgd_CreateDirectionalLight() {
	sgdx::started();
	auto light = new sgdx::Light(sgdx::LightType::directional);
	sgdx::mainScene()->add(light);
	return sgdx::createHandle(light);
}

SGD_Light SGD_DECL sgd_CreatePointLight() {
	sgdx::started();
	auto light = new sgdx::Light(sgdx::LightType::point);
	sgdx::mainScene()->add(light);
	return sgdx::createHandle(light);
}

SGD_Light SGD_DECL sgd_CreateSpotLight() {
	sgdx::started();
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

void SGD_DECL sgd_SetLightShadowsEnabled(SGD_Light hlight, SGD_Bool enabled) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);
	light->shadowsEnabled = enabled;
}

SGD_Bool SGD_DECL sgd_IsLightShadowsEnabled(SGD_Light hlight) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);
	return light->shadowsEnabled();
}

void SGD_DECL sgd_SetLightPriority(SGD_Light hlight, int priority) {
	auto light = sgdx::resolveHandle<sgdx::Light>(hlight);
	light->priority = priority;
}

// ***** Model *****

SGD_Model SGD_DECL sgd_LoadModel(SGD_String path) {
	auto mesh = sgdx::loadStaticMesh(sgdx::Path(path));
	if (!mesh) sgdx::error("Failed to load mesh", mesh.error());
	auto model = new sgd::Model();
	model->mesh = mesh.result();
	sgdx::mainScene()->add(model);
	return sgdx::createHandle(model);
}

SGD_Model SGD_DECL sgd_LoadBonedModel(SGD_String path, SGD_Bool skinned) {
	sgd::Model* model;
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

void SGD_DECL sgd_AnimateModel(SGD_Model hmodel, int animation, float time, int mode, float weight) {
	auto model = sgdx::resolveHandle<sgd::Model>(hmodel);
	model->animate(animation, time, (sgdx::AnimationMode)mode, weight);
}

SGD_Model SGD_DECL sgd_CreateModel(SGD_Mesh hmesh) {
	auto mesh = hmesh ? sgdx::resolveHandle<sgdx::Mesh>(hmesh) : nullptr;
	auto model = new sgd::Model();
	sgdx::mainScene()->add(model);
	if (mesh) model->mesh = mesh;
	return sgdx::createHandle(model);
}

void SGD_DECL sgd_SetModelMesh(SGD_Model hmodel, SGD_Mesh hmesh) {
	auto model = sgdx::resolveHandle<sgd::Model>(hmodel);
	auto mesh = sgdx::resolveHandle<sgdx::Mesh>(hmesh);
	model->mesh = mesh;
}

SGD_Mesh SGD_DECL sgd_GetModelMesh(SGD_Model hmodel) {
	auto model = sgdx::resolveHandle<sgd::Model>(hmodel);
	auto mesh = model->mesh().get();
	return mesh ? sgdx::getOrCreateHandle(mesh) : 0;
}

void SGD_DECL sgd_SetModelColor(SGD_Model hmodel, float red, float green, float blue, float alpha) {
	auto model = sgdx::resolveHandle<sgd::Model>(hmodel);
	model->color = sgdx::Vec4f(red, green, blue, alpha);
}

// ***** Sprite *****

SGD_Sprite SGD_DECL sgd_CreateSprite(SGD_Image himage) {
	auto image = himage ? sgdx::resolveHandle<sgdx::Image>(himage) : nullptr;
	auto sprite = new sgdx::Sprite();
	sgdx::mainScene()->add(sprite);
	sprite->image = image;
	return sgdx::createHandle<sgdx::Sprite>(sprite);
}

void SGD_DECL sgd_SetSpriteImage(SGD_Sprite hsprite, SGD_Image himage) {
	sgdx::resolveHandle<sgdx::Sprite>(hsprite)->image = sgdx::resolveHandle<sgdx::Image>(himage);
}

void SGD_DECL sgd_SetSpriteColor(SGD_Sprite hsprite, float red, float green, float blue, float alpha) {
	sgdx::resolveHandle<sgdx::Sprite>(hsprite)->color = {red, green, blue, alpha};
}

void SGD_DECL sgd_SetSpriteFrame(SGD_Sprite hsprite, float frame) {
	sgdx::resolveHandle<sgdx::Sprite>(hsprite)->frame = frame;
}

// ***** Skybox *****

SGD_Skybox SGD_DECL sgd_LoadSkybox(SGD_String path, float roughness) {
	sgdx::started();
	auto flags = sgd::TextureFlags::mipmap | sgd::TextureFlags::filter;
	auto texture = sgd::loadCubeTexture(sgdx::Path(path), sgdx::TextureFormat::any, flags);
	if (!texture) sgdx::error("Failed to load texture", texture.error());

	auto skybox = new sgdx::Skybox();
	skybox->skyTexture = texture.result();
	skybox->roughness = roughness;

	sgdx::mainScene()->add(skybox);

	return sgdx::createHandle(skybox);
}

SGD_Skybox SGD_DECL sgd_CreateSkybox(SGD_Texture htexture) {
	auto texture = htexture ? sgdx::resolveHandle<sgdx::Texture>(htexture) : nullptr;
	auto skybox = new sgdx::Skybox();
	sgdx::mainScene()->add(skybox);
	if (texture) skybox->skyTexture = texture;
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

// ***** Collisions *****

SGD_Collider SGD_DECL sgd_CreateSphereCollider(SGD_Entity hentity, int colliderType, float radius) {
	if((uint32_t)colliderType>31) sgdx::error("ColliderType must be in the range 0..31");
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto collider = new sgd::SphereCollider(entity, (uint32_t)colliderType, radius);
	return sgdx::createHandle(collider);
}

SGD_Collider SGD_DECL sgd_CreateEllipsoidCollider(SGD_Entity hentity, int colliderType, float radius, float height) {
	if((uint32_t)colliderType>31) sgdx::error("ColliderType must be in the range 0..31");
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	auto collider = new sgd::EllipsoidCollider(entity, (uint32_t)colliderType, radius, height);
	return sgdx::createHandle(collider);
}

SGD_Collider SGD_DECL sgd_CreateMeshCollider(SGD_Entity hentity, int colliderType, SGD_Mesh hmesh) {
	if((uint32_t)colliderType>31) sgdx::error("ColliderType must be in the range 0..31");
	auto entity = sgdx::resolveHandle<sgd::Entity>(hentity);
	sgd::CMesh* mesh = hmesh ? sgdx::resolveHandle<sgd::Mesh>(hmesh) : nullptr;
	if (!mesh) {
		if (!entity->is<sgd::Model>()) sgdx::error("Entity must be a model");
		mesh = entity->as<sgd::Model>()->mesh();
		if (!mesh) sgdx::error("Model must have a valid mesh");
	}
	auto data = sgd::getOrCreateMeshColliderData(mesh);
	auto collider = new sgd::MeshCollider(entity, (uint32_t)colliderType, data);
	return sgdx::createHandle(collider);
}

SGD_Entity SGD_DECL sgd_GetColliderEntity(SGD_Collider hcollider) {
	return sgdx::getOrCreateHandle(sgdx::resolveHandle<sgd::Collider>(hcollider)->entity());
}

int SGD_DECL sgd_GetColliderType(SGD_Collider hcollider) {
	return (int)sgdx::resolveHandle<sgd::Collider>(hcollider)->colliderType();
}

void SGD_DECL sgd_SetColliderRadius(SGD_Collider hcollider, float radius) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if (collider->is<sgd::SphereCollider>()) {
		collider->as<sgd::SphereCollider>()->radius = radius;
	} else if (collider->is<sgd::EllipsoidCollider>()) {
		collider->as<sgd::EllipsoidCollider>()->radius = radius;
	} else {
		sgdx::error("Collider does not have a radius property");
	}
}

void SGD_DECL sgd_SetColliderHeight(SGD_Collider hcollider, float height) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if (collider->is<sgd::EllipsoidCollider>()) {
		collider->as<sgd::EllipsoidCollider>()->height = height;
	} else {
		sgdx::error("Collider does not have a height property");
	}
}

void SGD_DECL sgd_EnableCollisions(int srcColliderType, int dstColliderType, int collisionRepsonse) {
	sgdx::mainScene()->collisionSpace()->enableCollisions(srcColliderType, dstColliderType,
														  (sgd::CollisionResponse)collisionRepsonse);
}

void SGD_DECL sgd_UpdateColliders() {
	sgdx::mainScene()->collisionSpace()->updateColliders();
}

int SGD_DECL sgd_GetCollisionCount(SGD_Collider hcollider) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	return (int)collider->collisions().size();
}

SGD_Collider SGD_DECL sgd_GetCollisionCollider(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return sgdx::getOrCreateHandle(collider->collisions()[index].collider.get());
}

SGD_Real SGD_DECL sgd_GetCollisionX(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return collider->collisions()[index].contact.point.x;
}

SGD_Real SGD_DECL sgd_GetCollisionY(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return collider->collisions()[index].contact.point.y;
}

SGD_Real SGD_DECL sgd_GetCollisionZ(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return collider->collisions()[index].contact.point.z;
}

SGD_Real SGD_DECL sgd_GetCollisionNX(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return collider->collisions()[index].contact.normal.x;
}

SGD_Real SGD_DECL sgd_GetCollisionNY(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return collider->collisions()[index].contact.normal.y;
}

SGD_Real SGD_DECL sgd_GetCollisionNZ(SGD_Collider hcollider, int index) {
	auto collider = sgdx::resolveHandle<sgd::Collider>(hcollider);
	if ((uint32_t)index >= collider->collisions().size()) sgdx::error("Collision index out of range");
	return collider->collisions()[index].contact.normal.z;
}

SGD_Collider SGD_DECL sgd_CameraPick(SGD_Camera hcamera, float windowX, float windowY, int colliderMask) {
	auto camera = sgdx::resolveHandle<sgd::Camera>(hcamera);
	sgd::Contact contact;
	auto collider = sgd::intersectRay(camera, {windowX, windowY}, colliderMask, contact);
	if (collider) g_picked = contact;
	return collider ? sgdx::getOrCreateHandle(collider) : 0;
}

SGD_Collider SGD_DECL sgd_LinePick(SGD_Real x0, SGD_Real y0, SGD_Real z0, SGD_Real x1, SGD_Real y1, SGD_Real z1,
								   SGD_Real radius, int colliderMask) {
	sgd::Vec3r src(x0, y0, z0);
	sgd::Vec3r dst(x1, y1, z1);
	auto dir = dst - src;
	auto d = sgd::length(dir);
	if (d == 0) return false;
	sgd::Liner ray(src, dir / d);
	sgd::Contact contact(d);
	auto collider = sgdx::mainScene()->collisionSpace()->intersectRay(ray, radius, colliderMask, contact);
	if (collider) g_picked = contact;
	return collider ? sgdx::getOrCreateHandle(collider) : 0;
}

SGD_Real SGD_DECL sgd_GetPickedX() {
	return g_picked.point.x;
}

SGD_Real SGD_DECL sgd_GetPickedY() {
	return g_picked.point.y;
}

SGD_Real SGD_DECL sgd_GetPickedZ() {
	return g_picked.point.z;
}

SGD_Real SGD_DECL sgd_GetPickedNX() {
	return g_picked.normal.x;
}

SGD_Real SGD_DECL sgd_GetPickedNY() {
	return g_picked.normal.y;
}

SGD_Real SGD_DECL sgd_GetPickedNZ() {
	return g_picked.normal.z;
}

// ***** Render effects *****

SGD_RenderEffect SGD_DECL sgd_CreateBloomEffect() {
	auto effect = new sgd::BloomEffect();
	sgdx::mainScene()->sceneRenderer()->add(effect);
	return sgdx::createHandle(effect);
}

void SGD_DECL sgd_SetBloomEffectRadius(SGD_RenderEffect heffect, int radius) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::BloomEffect>()) sgdx::error("Effect is not a bloom effect");
	if (radius < 1 || radius > 31) sgdx::error("Bloom radius must be in the range 1 to 31 inclusive");
	effect->as<sgd::BloomEffect>()->radius = radius;
}

int SGD_DECL sgd_GetBloomEffectRadius(SGD_RenderEffect heffect) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::BloomEffect>()) sgdx::error("Effect is not a bloom effect");
	return (int)effect->as<sgd::BloomEffect>()->radius();
}

SGD_RenderEffect SGD_DECL sgd_CreateBlurEffect() {
	auto effect = new sgd::BlurEffect();
	sgdx::mainScene()->sceneRenderer()->add(effect);
	return sgdx::createHandle(effect);
}

void SGD_DECL sgd_SetBlurEffectRadius(SGD_RenderEffect heffect, int radius) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::BlurEffect>()) sgdx::error("Effect is not a blur effect");
	if (radius < 1 || radius > 31) sgdx::error("Blur radius must be in the range 1 to 31 inclusive");
	effect->as<sgd::BlurEffect>()->radius = radius;
}

int SGD_DECL sgd_GetBlurEffectRadius(SGD_RenderEffect heffect) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::BlurEffect>()) sgdx::error("Effect is not a blur effect");
	return (int)effect->as<sgd::BlurEffect>()->radius();
}

SGD_RenderEffect SGD_DECL sgd_CreateFogEffect() {
	auto effect = new sgd::FogEffect();
	sgdx::mainScene()->sceneRenderer()->add(effect);
	return sgdx::createHandle(effect);
}

void SGD_DECL sgd_SetFogEffectColor(SGD_RenderEffect heffect, float red, float green, float blue, float alpha) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::FogEffect>()) sgdx::error("Effect is not a fog effect");
	effect->as<sgd::FogEffect>()->color = {red, green, blue, alpha};
}

void SGD_DECL sgd_SetFogEffectRange(SGD_RenderEffect heffect, float near, float far) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::FogEffect>()) sgdx::error("Effect is not a fog effect");
	effect->as<sgd::FogEffect>()->near = near;
	effect->as<sgd::FogEffect>()->far = far;
}

void SGD_DECL sgd_SetFogEffectPower(SGD_RenderEffect heffect, float power) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::FogEffect>()) sgdx::error("Effect is not a fog effect");
	effect->as<sgd::FogEffect>()->power = power;
}

SGD_RenderEffect SGD_DECL sgd_CreateMonocolorEffect() {
	auto effect = new sgd::MonocolorEffect();
	sgdx::mainScene()->sceneRenderer()->add(effect);
	return sgdx::createHandle(effect);
}

void SGD_DECL sgd_SetMonocolorEffectColor(SGD_RenderEffect heffect, float red, float green, float blue, float alpha) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	if (!effect->is<sgd::MonocolorEffect>()) sgdx::error("Effect is not a monocolor effect");
	effect->as<sgd::MonocolorEffect>()->color = sgd::Vec4f(red, green, blue, alpha);
}

void SGD_DECL sgd_SetRenderEffectEnabled(SGD_RenderEffect heffect, SGD_Bool enabled) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	effect->enabled = enabled;
}

//! Is render effect enabled.
SGD_Bool SGD_DECL sgd_IsRenderEffectEnabled(SGD_RenderEffect heffect) {
	auto effect = sgdx::resolveHandle<sgd::RenderEffect>(heffect);
	return effect->enabled();
}
