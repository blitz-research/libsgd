#include "start.c"

void entry() {

	sgd_SetSceneAmbientLightColor(0,0,0,1);

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 0, 0);

	SGD_Material material = sgd_LoadMaterial("sgd://assets/Bricks076C_1K-JPG");
	SGD_Mesh mesh = sgd_CreateBoxMesh(-10, -10, -1, 10, 10, 1, material);
	sgd_TransformMeshTexCoords(mesh, 2,2,0,0);
	SGD_Model model = sgd_CreateModel();
	sgd_SetModelMesh(model, mesh);
	sgd_MoveEntity(model, 0, 0, 10);

	// Pointing rightwards
	SGD_Light light0 = sgd_CreateSpotLight();
	sgd_SetLightInnerConeAngle(light0, 30);
	sgd_SetLightOuterConeAngle(light0, 45);
	sgd_MoveEntity(light0,0,0,5);

	for (;;) {
		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

//		sgd_TurnEntity(model, 0, .01, 0);

		sgd_RenderScene();

		sgd_Present();
	}

	exit(0);
}

int main() {
	start(entry);
}
