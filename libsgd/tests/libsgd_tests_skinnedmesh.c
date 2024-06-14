#include "start.c"

void entry() {

restart:

	reset();

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 1, -3);

	SGD_Light light = sgd_CreatePointLight();
	sgd_MoveEntity(light, 0, 3, -3);
	sgd_SetLightRange(light, 10);

	SGD_Model model = sgd_LoadBonedModel("sgd://models/larry_walk.glb", SGD_TRUE);

	SGD_Material material = sgd_LoadPBRMaterial("sgd://materials/PavingStones065_1K-JPG");
	SGD_Mesh mesh = sgd_CreateBoxMesh(-15, -1, -10, 10, 0, 10, material);
	sgd_TFormTexCoords(mesh, 7.5, 7.5, 0, 0);
	SGD_Model ground = sgd_CreateModel(mesh);

	sgd_DebugMemory();

	for (;;) {

		int mask = sgd_PollEvents();

		if (mask & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		// Gamepad...
		if (sgd_GetGamepadButtonDown(0, SGD_GAMEPAD_BUTTON_LEFT_BUMPER)) {
			sgd_TurnEntity(model, 0, 3, 0);
		} else if (sgd_GetGamepadButtonDown(0, SGD_GAMEPAD_BUTTON_RIGHT_BUMPER)) {
			sgd_TurnEntity(model, 0, -3, 0);
		}

		// Keyboard...
		if (sgd_GetKeyDown(SGD_KEY_LEFT)) {
			sgd_TurnEntity(model, 0, 3, 0);
		} else if (sgd_GetKeyDown(SGD_KEY_RIGHT)) {
			sgd_TurnEntity(model, 0, -3, 0);
		}

		if (sgd_GetKeyDown(SGD_KEY_UP)) {
			sgd_MoveEntity(model, 0, 0, -.03f);
		} else if (sgd_GetKeyDown(SGD_KEY_DOWN)) {
			sgd_MoveEntity(model, 0, 0, .03f);
		}

		static float time;
		sgd_AnimateModel(model, 0, time += .017f, SGD_ANIMATION_MODE_LOOP, 1);

		char buf[80];
		sprintf(buf, "FPS:%f", sgd_GetFPS());
		sgd_Clear2D();
		sgd_Draw2DText(buf, 0, 0);

		sgd_RenderScene();
		sgd_Present();
	}
}

int main() {

	start(entry);
}
