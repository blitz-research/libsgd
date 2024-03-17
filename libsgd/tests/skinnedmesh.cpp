
void start() {

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 1, -3);

	SGD_Light light = sgd_CreateDirectionalLight();

	SGD_Model model = sgd_LoadBonedModel("cesiumman.glb", SGD_TRUE);

	for (;;) {

		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		// Gamepad...
		if (sgd_GamepadButtonDown(0, SGD_GAMEPAD_BUTTON_LEFT_BUMPER)) {
			sgd_TurnEntity(model, 0, .6, 0);
		} else if (sgd_GamepadButtonDown(0, SGD_GAMEPAD_BUTTON_RIGHT_BUMPER)) {
			sgd_TurnEntity(model, 0, -.6, 0);
		}

		// Keyboard...
		if (sgd_KeyDown(SGD_KEY_LEFT)) {
			sgd_TurnEntity(model, 0, .6, 0);
		} else if (sgd_KeyDown(SGD_KEY_RIGHT)) {
			sgd_TurnEntity(model, 0, -.6, 0);
		}

		static float time;
		sgd_AnimateModel(model, 0, time += .01f, SGD_ANIMATION_MODE_LOOP);

		sgd_RenderScene();
		sgd_Present();
	}

	exit(0);
}
