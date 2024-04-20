#include <sgd/sgd.h>

#include <stdio.h>
#include <stdlib.h>

void reset() {

	sgd_ClearScene();

	SGD_Texture skyTexture = sgd_LoadTexture("sgd://envmaps/sunnysky-cube.png", SGD_TEXTURE_FORMAT_SRGBA8, SGD_TEXTURE_FLAGS_CUBE_MIPMAP);
	sgd_SetSceneEnvTexture(skyTexture);

	SGD_Skybox skybox = sgd_CreateSkybox();
	sgd_SetSkyboxTexture(skybox, skyTexture);
	sgd_SetSkyboxRoughness(skybox, .25f);
}

void start(void(*entry)()) {

	sgd_Init();

//	sgd_CreateWindow(sgd_DesktopWidth(), sgd_DesktopHeight(), "Hello LibSGD!", SGD_WINDOW_FLAGS_RESIZABLE|SGD_WINDOW_FLAGS_FULLSCREEN);
	sgd_CreateWindow(sgd_DesktopWidth()/2, sgd_DesktopHeight()*3/4, "Hello LibSGD!", SGD_WINDOW_FLAGS_RESIZABLE);

	sgd_SetMouseCursorMode(SGD_MOUSE_CURSOR_MODE_HIDDEN);

	sgd_CreateScene();

	reset();

	entry();

	sgd_Terminate();
}
