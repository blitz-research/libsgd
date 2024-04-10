#include "start.c"

void entry() {

restart:

	reset();

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 1, -3);

	SGD_Light light = sgd_CreatePointLight();
//	sgd_SetLightCastsShadow(light, SGD_TRUE);
	sgd_MoveEntity(light, 0, 3, -3);
	sgd_SetLightRange(light, 10);

	SGD_Model model = sgd_LoadBonedModel("sgd://models/cesiumman.glb", SGD_TRUE);
//	sgd_SetMeshCastsShadow(sgd_ModelMesh(model), SGD_TRUE);

	SGD_Material material = sgd_LoadPBRMaterial("sgd://materials/PavingStones065_1K-JPG");
	SGD_Mesh mesh = sgd_CreateBoxMesh(-10, -1, -10, 10, 0, 10, material);

	SGD_Model ground = sgd_CreateModel();
	sgd_SetModelMesh(ground,mesh);

	sgd_DebugMemory();

	int fps = 0;

	for (;;) {

		int mask = sgd_PollEvents();

		if (mask & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		if (mask & SGD_EVENT_MASK_SUSPENDED) puts("### App suspended");

		if (mask & SGD_EVENT_MASK_RESUMED) puts("### App resumed");

		fflush(stdout);

		// Gamepad...
		if (sgd_GamepadButtonDown(0, SGD_GAMEPAD_BUTTON_LEFT_BUMPER)) {
			sgd_TurnEntity(model, 0, 3, 0);
		} else if (sgd_GamepadButtonDown(0, SGD_GAMEPAD_BUTTON_RIGHT_BUMPER)) {
			sgd_TurnEntity(model, 0, -3, 0);
		}

		// Keyboard...
		if (sgd_KeyDown(SGD_KEY_LEFT)) {
			sgd_TurnEntity(model, 0, 3, 0);
		} else if (sgd_KeyDown(SGD_KEY_RIGHT)) {
			sgd_TurnEntity(model, 0, -3, 0);
		}

		static float time;
		sgd_AnimateModel(model, 0, time += .03f, SGD_ANIMATION_MODE_LOOP);

		sgd_RenderScene();
		sgd_Present();

		if (sgd_FPS() != fps) {
			fps = sgd_FPS();
			printf("### FPS: %i\n", fps);
			fflush(stdout);
		}

		sgd_AnimateModel(model, 0, 0, SGD_ANIMATION_MODE_LOOP);

		if (sgd_KeyHit(32)) {
			goto restart;
			//			sgd_DestroyEntity(model);
			//			model = sgd_LoadBonedModel("sgd://models/cesiumman.glb", 1);
		}
	}
}

int main() {

	start(entry);
}
