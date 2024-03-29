#include "start.c"

void entry() {

restart:

	reset();

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 1, -3);

	SGD_Light light = sgd_CreateDirectionalLight();

	SGD_Model model = sgd_LoadBonedModel("sgd://models/cesiumman.glb", 1);

	sgd_Debug();

	for (;;) {

		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

#if 0
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
#endif

		sgd_RenderScene();
		sgd_Present();

		if(sgd_KeyHit(32)) {
			//goto restart;
			sgd_DestroyEntity(model);
			model = sgd_LoadBonedModel("sgd://models/cesiumman.glb", 1);
		}

	}
}

int main() {

	start(entry);
}
