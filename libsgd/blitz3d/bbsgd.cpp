#include <sgd/sgd.h>

namespace {

void SGD_DECL errorHandler(SGD_String error, void *context) {
    RTEX(error);
}

void createWindow(int width, int height, BBStr *title, int flags) {
	sgd_CreateWindow(width, height, title->c_str(), flags);
	delete title;
}

SGD_Texture loadTexture(BBStr* path, int format, int flags) {
	auto texture = sgd_LoadTexture(path->c_str(), format, flags);
	delete path;

	return texture;
}

SGD_Material loadMaterial(BBStr* path) {
	auto material = sgd_LoadMaterial(path->c_str());
	delete path;

	return material;
}

SGD_Mesh loadMesh(BBStr* path) {
	auto mesh = sgd_LoadMesh(path->c_str());
	delete path;

	return mesh;
}

SGD_Model loadModel(BBStr* path) {
	auto model = sgd_LoadModel(path->c_str());
	delete path;

	return model;
}

SGD_Model loadBonedModel(BBStr* path,int skinned) {
	auto model = sgd_LoadBonedModel(path->c_str(), skinned);
	delete path;

	return model;
}

}

bool sgd_create(){
    sgd_SetErrorHandler(errorHandler, nullptr);
    return true;
}

bool sgd_destroy(){
    sgd_SetErrorHandler(nullptr, nullptr);
    return true;
}

bool sgd_link( void (*rtSym)( const char *sym, void *pc ) ){

    // Window
    rtSym("CreateWindow%width%height$title%flags", createWindow);
    rtSym("%PollEvents", sgd_PollEvents);
	rySym("%KeyDown%key",sgd_KeyDown);
	rySym("%KeyHit%key",sgd_KeyDown);
	rySym("%GetChar",sgd_GetChar);
	rtSym("#MouseX", sgd_MouseX);
	rtSym("#MouseY", sgd_MouseY);
	rtSym("#MouseScrollX", sgd_MouseX);
	rtSym("#MouseScrollY", sgd_MouseY);
	rtSym("%MouseButtonDown%button",sgd_MouseButtonDown);
	rtSym("%MouseButtonHit%button",sgd_MouseButtonHit);
	rtSym("%GamepadConnected%gamepad",sgd_GamepadConnected);
	rtSym("%GamepadButtonDown%gamepad%button",sgd_GamepadButtonDown);
	rtSym("%GamepadButtonHit%gamepad%button",sgd_GamepadButtonHit);
	rtSym("#GamepadAxis%gamepad%axis",sgd_GamepadAxis);

    // Scene
    rtSym("CreateScene", sgd_CreateScene);
    rtSym("SetSceneClearColor#red#green#blue#alpha", sgd_SetSceneClearColor);
    rtSym("SetSceneClearDepth#depth", sgd_SetSceneClearDepth);
    rtSym("SetSceneAmbientLightColor#red#green#blue#alpha", sgd_SetSceneAmbientLightColor);
    rtSym("SetSceneEnvTexture%texture", sgd_SetSceneEnvTexture);
    rtSym("RenderScene", sgd_RenderScene);
    rtSym("Present", sgd_Present);

    // Texture
    rtSym("%LoadTexture$path%format%flags", loadTexture);

    // Material
    rtSym("%LoadMaterial$path", loadMaterial);

    // Mesh
	rtSym("%LoadMesh$path", loadMesh);
	rtSym("%CreateBoxMesh#minX#minY#minZ#maxX#maxY#maxZ%material", sgd_CreateBoxMesh);
	rtSym("%CreateSphereMesh#radius%hSegs%vSegs%material", sgd_CreateSphereMesh);
	rtSym("FitMesh%mesh#minX#minY#minZ#maxX#maxY#maxZ%uniform", sgd_FitMesh);
    rtSym("TransformMeshTexCoords%mesh#scaleU#scaleV#offsetU#offsetV", sgd_TransformMeshTexCoords);

    // Skybox
	rtSym("%LoadSkybox$path#roughness", sgd_LoadSkybox);
    rtSym("%CreateSkybox", sgd_CreateSkybox);
    rtSym("SetSkyboxTexture%skybox%texture", sgd_SetSkyboxTexture);
	rtSym("SetSkyboxRoughness%skybox#roughness", sgd_SetSkyboxRoughness);

    // Model
	rtSym("%LoadModel$path", loadModel);
	rtSym("%LoadBonedModel$path%skinned", loadBonedModel);
	rtSym("AnimateModel", sgd_AnimateModel);
    rtSym("%CreateModel", sgd_CreateModel);
    rtSym("SetModelMesh%model%mesh", sgd_SetModelMesh);
    rtSym("SetModelColor%model#red#green#blue#alpha", sgd_SetModelColor);
	rtSym("AnimateModel%model%animation#time%mode", sgd_animateModel);

	// Camera
	rtSym("%CreatePerspectiveCamera", sgd_CreatePerspectiveCamera);
	rtSym("%CreateOrthographicCamera", sgd_CreateOrthographicCamera);
	rtSym("SetCameraFOV%camera#fov", sgd_SetCameraFOV);
	rtSym("SetCameraNear%camera#near", sgd_SetCameraNear);
	rtSym("SetCameraFar%camera#far", sgd_SetCameraFar);

    // Light
    rtSym("%CreateDirectionalLight", sgd_CreateDirectionalLight);
    rtSym("%CreatePointLight", sgd_CreatePointLight);
    rtSym("%CreateSpotLight", sgd_CreateSpotLight);
    rtSym("SetLightColor%light#red#green#blue#alpha", sgd_SetLightColor);
    rtSym("SetLightRange%light#range", sgd_SetLightRange);
    rtSym("SetLightFalloff%light#falloff", sgd_SetLightFalloff);
	rtSym("SetLightInnerConeAngle%light#angle", sgd_SetLightInnerConeAngle);
	rtSym("SetLightOuterConeAngle%light#angle", sgd_SetLightOuterConeAngle);

	// Entity
	rtSym("%CopyEntity%entity", sgd_CopyEntity);
	rtSym("SetEntityParent%entity%parent", sgd_SetEntityParent);
	rtSym("%EntityParent%entity", sgd_EntityParent);

	rtSym("SetEntityPosition%entity#x#y#z", sgd_SetEntityPosition);
	rtSym("SetEntityRotation%entity#pitch#yaw#roll", sgd_SetEntityRotation);
	rtSym("MoveEntity%entity#x#y#z", sgd_MoveEntity);
	rtSym("TurnEntity%entity#pitch#yaw#roll", sgd_TurnEntity);
	rtSym("TranslateEntity%entity#x#y#z", sgd_TranslateEntity);
	rtSym("RotateEntity%entity#pitch#yaw#roll", sgd_RotateEntity);

	rtSym("#EntityX%entity", sgd_EntityX);
	rtSym("#EntityY%entity", sgd_EntityY);
	rtSym("#EntityZ%entity", sgd_EntityZ);
	rtSym("#EntityRX%entity", sgd_EntityRX);
	rtSym("#EntityRY%entity", sgd_EntityRY);
	rtSym("#EntityRZ%entity", sgd_EntityRZ);

    return true;
}
