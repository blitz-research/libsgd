#include <sgd/sgd.h>

#include <stdio.h>
#include <stdlib.h>

//#include "directionallights.cpp"
#include "skinnedmesh.cpp"
//#include "spotlight.cpp"

void SGD_DECL init() {
	sgd_CreateWindow(1280, 720, "Hello LibSGD!", SGD_WINDOW_FLAGS_RESIZABLE);

	sgd_SetWebGPUBackend("D3D11");

	sgd_CreateScene();

	SGD_Texture skyTexture = sgd_LoadTexture("sunnysky-cube.png", SGD_TEXTURE_FORMAT_SRGBA8, SGD_TEXTURE_FLAGS_CUBE_MIPMAP);
	sgd_SetSceneEnvTexture(skyTexture);

	SGD_Skybox skybox = sgd_CreateSkybox();
	sgd_SetSkyboxTexture(skybox, skyTexture);
	sgd_SetSkyboxRoughness(skybox, .25f);

	start();
}

int main() {
	init();
//	sgd_Run(init);

}
