#include <sgd/sgd.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void start(void (*entry)()) {

	sgd_Init();

	sgd_CreateWindow(sgd_GetDesktopWidth() / 2, sgd_GetDesktopHeight() * 3 / 4, "Hello LibSGD!",
					 SGD_WINDOW_FLAGS_RESIZABLE | SGD_WINDOW_FLAGS_CENTERED);

	SGD_Texture skyTexture =
		sgd_LoadCubeTexture("sgd://envmaps/sunnysky-cube.png", SGD_TEXTURE_FORMAT_ANY, SGD_TEXTURE_FLAGS_DEFAULT);

	sgd_SetEnvTexture(skyTexture);

	sgd_CreateSkybox(skyTexture);

	entry();

	sgd_Terminate();
}
