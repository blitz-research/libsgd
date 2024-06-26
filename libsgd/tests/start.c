#include <sgd/sgd.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void reset() {

	sgd_ClearScene();

	SGD_Texture skyTexture = sgd_LoadTexture("sgd://envmaps/sunnysky-cube.png", SGD_TEXTURE_FORMAT_SRGBA8, SGD_TEXTURE_FLAGS_ENVMAP_DEFAULT);
	sgd_SetEnvTexture(skyTexture);

	SGD_Skybox skybox = sgd_CreateSkybox(skyTexture);
	sgd_SetSkyboxRoughness(skybox, .25f);
}

void start(void(*entry)()) {

	sgd_Init();

	sgd_CreateWindow(sgd_GetDesktopWidth()/2, sgd_GetDesktopHeight()*3/4, "Hello LibSGD!", SGD_WINDOW_FLAGS_RESIZABLE|SGD_WINDOW_FLAGS_CENTERED);

	reset();

	entry();

	sgd_Terminate();
}
