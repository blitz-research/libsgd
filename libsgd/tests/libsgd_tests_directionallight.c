#include "start.c"

void entry() {

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 0, -3);

	SGD_Material material = sgd_LoadPBRMaterial("sgd://materials/Bricks076C_1K-JPG");
	SGD_Mesh mesh = sgd_CreateSphereMesh(1, 64, 32, material);
	sgd_TransformMeshTexCoords(mesh, 2, 2, 0, 0);
	SGD_Model model = sgd_CreateModel();
	sgd_SetModelMesh(model, mesh);

	// Pointing rightwards
	SGD_Light light0 = sgd_CreateDirectionalLight();
	sgd_SetLightColor(light0, 1, 0, 0, 1);
	sgd_TurnEntity(light0, 0, -90, 0);

	// Pointing leftwards
	SGD_Light light1 = sgd_CreateDirectionalLight();
	sgd_SetLightColor(light1, 0, 1, 0, 1);
	sgd_TurnEntity(light1, 0, 90, 0);

	// Pointing downwards
	SGD_Light light2 = sgd_CreateDirectionalLight();
	sgd_SetLightColor(light2, 0, 0, 1, 1);
	sgd_TurnEntity(light2, -90, 0, 0);

	// Pointing upwards
	SGD_Light light3 = sgd_CreateDirectionalLight();
	sgd_SetLightColor(light3, 1, 1, 1, 1);
	sgd_TurnEntity(light3, 90, 0, 0);

	puts("Entering render loop");

	for (;;) {
		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		sgd_TurnEntity(model, 0, .1f, 0);

		sgd_RenderScene();

		sgd_Present();
	}

	exit(0);
}

int main() {
	start(entry);
}
