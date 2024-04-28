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
	sgd_TransformMeshTexCoords(mesh, 4, 4, 0, 0);
	SGD_Model ground = sgd_CreateModel();
	sgd_SetModelMesh(ground, mesh);

	sgd_DebugMemory();

///	SGD_Sound sound = sgd_LoadSound("sgd://audio/fine_morning.ogg");
	SGD_Sound sound = sgd_LoadSound("sgd://audio/slimeball.wav");

	for (;;) {

		int mask = sgd_PollEvents();

		if (mask & SGD_EVENT_MASK_CLOSE_CLICKED) break;

//		if (mask & SGD_EVENT_MASK_SUSPENDED) puts("### App suspended");
//		if (mask & SGD_EVENT_MASK_RESUMED) puts("### App resumed");

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

		if (sgd_KeyDown(SGD_KEY_UP)) {
			sgd_MoveEntity(model, 0, 0, -.03);
		} else if (sgd_KeyDown(SGD_KEY_DOWN)) {
			sgd_MoveEntity(model, 0, 0, .03);
		}

		static float time;
		sgd_AnimateModel(model, 0, time += .03f, SGD_ANIMATION_MODE_LOOP);

		sgd_RenderScene();
		sgd_Present();

		sgd_AnimateModel(model, 0, 0, SGD_ANIMATION_MODE_LOOP);

		//		if (sgd_KeyHit(32)) {
		//			goto restart;
		// sgd_DestroyEntity(model);
		// model = sgd_LoadBonedModel("sgd://models/cesiumman.glb", 1);
		//		}

		if (sgd_KeyHit(SGD_KEY_SPACE)) {
			sgd_PlaySound(sound);
//			int audio = sgd_CueSound(sound);
//			sgd_SetAudioPan(audio, ((float)rand() / RAND_MAX) * 2 -1);
//			sgd_SetAudioPaused(audio, SGD_FALSE);
		}

		char buf[80];
		sprintf(buf, "FPS:%f", sgd_FPS());
		sgd_Clear2D();
		sgd_Draw2DText(buf, 0, 0);
	}
}

int main() {

	start(entry);
}
