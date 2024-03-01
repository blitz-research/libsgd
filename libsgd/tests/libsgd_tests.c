#include <sgd/sgd.h>

#include <stdio.h>
#include <stdlib.h>

void SGD_DECL start() {
	sgd_CreateWindow(1280, 720, "Hello LibSGD!", SGD_WINDOW_FLAGS_RESIZABLE);

	sgd_CreateScene();

	SGD_Texture skyTexture = sgd_LoadTexture("skybox-4k.png", 2, 0x807);
	sgd_SetSceneEnvTexture(skyTexture);

	SGD_Skybox skybox= sgd_CreateSkybox();
	sgd_SetSkyboxTexture(skybox, skyTexture);

	SGD_Material material = sgd_LoadMaterial("Marble008_1K-JPG");
	SGD_Mesh mesh = sgd_CreateSphereMesh(1, 96, 48, material);
	sgd_TransformMeshTexCoords(mesh, 4, 2, 0, 0);
	SGD_Model model = sgd_CreateModel();
	sgd_MoveEntity(model, 0, 0, 3);
	sgd_SetModelMesh(model, mesh);

	SGD_Light light = sgd_CreateLight();
	sgd_MoveEntity(light, 0, 1.5, 0);
	sgd_SetLightRange(light, 10);

	puts("Entering render loop");

	for (;;) {
		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		sgd_TurnEntity(model, 0, .01, 0);

		sgd_RenderScene();

		sgd_Present();
	}

	exit(0);
}

int main() {
	sgd_Run(start);
}
