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
	rtSym("FitMesh%mesh#minX#minY#minZ#maxX#maxY#maxZ%uniform", sgd_FitMesh);
    rtSym("TransformMeshTexCoords%mesh#scaleU#scaleV#offsetU#offsetV", sgd_TransformMeshTexCoords);
    rtSym("%CreateBoxMesh#minX#minY#minZ#maxX#maxY#maxZ%material", sgd_CreateBoxMesh);
    rtSym("%CreateSphereMesh#radius%hSegs%vSegs%material", sgd_CreateSphereMesh);

    // Skybox
    rtSym("%CreateSkybox", sgd_CreateSkybox);
    rtSym("SetSkyboxTexture%skybox%texture", sgd_SetSkyboxTexture);
	rtSym("SetSkyboxRoughness%skybox#roughness", sgd_SetSkyboxRoughness);

    // Model
    rtSym("%CreateModel", sgd_CreateModel);
    rtSym("SetModelMesh%model%mesh", sgd_SetModelMesh);
    rtSym("SetModelColor%model#red#green#blue#alpha", sgd_SetModelColor);

    // Light
    rtSym("%CreateLight", sgd_CreateLight);
    rtSym("SetLightColor%light#red#green#blue#alpha", sgd_SetLightColor);
    rtSym("SetLightRange%light#range", sgd_SetLightRange);
    rtSym("SetLightFalloff%light#falloff", sgd_SetLightFalloff);

    rtSym("MoveEntity%entity#x#y#z", sgd_MoveEntity);
    rtSym("TurnEntity%entity#pitch#yawy#roll", sgd_TurnEntity);

    return true;
}
