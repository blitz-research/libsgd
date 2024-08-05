#include "start.c"

void entry() {

	sgd_SetAmbientLightColor(0, 0, 0, 1);

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_TurnEntity(camera, -30, 0, 0);

	SGD_Material material = sgd_CreatePBRMaterial();

	SGD_Mesh mesh = sgd_CreateBoxMesh(-10, -10, -10, 10, 10, 10, material);
	sgd_FlipMesh(mesh);
	SGD_Model model = sgd_CreateModel(mesh);

	sgd_SetMaxCSMLights(2);

	SGD_Mesh mesh2 = sgd_CreateSphereMesh(1, 24, 12, material);
	sgd_SetMeshShadowsEnabled(mesh2, 1);

	SGD_Model model2 = sgd_CreateModel(mesh2);
	sgd_MoveEntity(model2, 0, 0, 0);

	SGD_Model pivot = sgd_CreateModel(0);
	sgd_MoveEntity(pivot, 0, 3, 0);

	// Red, left
	SGD_Light light0 = sgd_CreatePointLight();
	sgd_SetEntityParent(light0, pivot);
	sgd_SetLightShadowsEnabled(light0, 1);
	sgd_MoveEntity(light0, -5, 0, 0);
	sgd_SetLightColor(light0, 1, 0, 0, 1.5);
	sgd_SetLightRange(light0, 20);

	// Green, right
	SGD_Light light1 = sgd_CreatePointLight();
	sgd_SetEntityParent(light1, pivot);
	sgd_SetLightShadowsEnabled(light1, 1);
	sgd_MoveEntity(light1, 5, 0, 0);
	sgd_SetLightColor(light1, 0, 1, 0, 1.5);
	sgd_SetLightRange(light1, 20);
#if 0
	// Blue, left front
	SGD_Light light2 = sgd_CreatePointLight();
	sgd_SetEntityParent(light2, pivot);
	sgd_SetLightShadowMappingEnabled(light2, 1);
  	sgd_MoveEntity(light2, -5, 5, -5);
	sgd_SetLightColor(light2, 0, 0, 1, 1);
	sgd_SetLightRange(light2, 25);
#endif

#if 0
	// white, right front
	SGD_Light light3 = sgd_CreatePointLight();
	sgd_SetLightShadowMappingEnabled(light3, 1);
	sgd_MoveEntity(light3, 5, 5, -5);
	sgd_SetLightColor(light3, 1, 1, 1, 1);
	sgd_SetLightRange(light3, 25);
#endif

	for (;;) {
		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		sgd_TurnEntity(pivot, 0, .1f, 0);

		sgd_SetEntityPosition(camera, 0, 0, 0);
		if (sgd_IsKeyDown(SGD_KEY_LEFT)) {
			sgd_RotateEntity(camera, 0, -1, 0);
		} else if (sgd_IsKeyDown(SGD_KEY_RIGHT)) {
			sgd_RotateEntity(camera, 0, 1, 0);
		}
		sgd_MoveEntity(camera, 0, 0, -10);

		sgd_RenderScene();
		sgd_Present();
	}

	exit(0);
}

int main() {
	start(entry);
}
