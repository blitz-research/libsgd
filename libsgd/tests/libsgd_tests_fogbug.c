#include <sgd/sgd.h>

int main(int argc, char *argv[]) {

	sgd_Init();

	sgd_CreateWindow(1280, 720, "Fog test", SGD_WINDOW_FLAGS_CENTERED);

	sgd_SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	sgd_SetAmbientLightColor(0.15f, 0.15f, 0.15f, 1.0f);

	const float viewRange = 15.0f;

	SGD_Texture env = sgd_LoadCubeTexture("sgd://envmaps/sunnysky-cube.png", SGD_TEXTURE_FORMAT_ANY, SGD_TEXTURE_FLAGS_DEFAULT);
	sgd_SetEnvTexture(env);

	SGD_Skybox skybox = sgd_CreateSkybox(env);
	sgd_SetSkyboxRoughness(skybox, 0.3f);

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_SetCameraNear(camera, 0.1f);
	sgd_SetCameraFar(camera, viewRange);
	sgd_SetEntityPosition(camera, 0.0f, 0.0f, -5.0f);

	SGD_Light light = sgd_CreateDirectionalLight();
	sgd_SetLightShadowsEnabled(light, 1);
	sgd_TurnEntity(light, -90.0f, 0.0f, 0.0f);

	SGD_RenderEffect fogEffect = sgd_CreateFogEffect();
	sgd_SetFogEffectColor(fogEffect, 0.5f, 0.5f, 0.5f, 1.0f);
	sgd_SetFogEffectRange(fogEffect, 0.1f, viewRange);
	sgd_SetFogEffectPower(fogEffect, 0.5f);
	sgd_SetRenderEffectEnabled(fogEffect, 1);
	sgd_SetRenderEffectEnabled(fogEffect, 0);		// Causes crash on MacOS.

	SGD_Material material = sgd_LoadPBRMaterial("sgd://materials/Fabric048_1K-JPG");
	SGD_Mesh mesh = sgd_CreateBoxMesh(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, material);

	SGD_Model model = sgd_CreateModel(mesh);
	sgd_SetEntityPosition(model, 0.0f, 0.0f, 0.0f);

	sgd_Clear2D();
	sgd_Set2DTextColor(1.0f, 0.0f, 0.0f, 1.0f);
	sgd_Draw2DText("Hello world! SGD is great!",  (sgd_GetWindowWidth() / 2.0f), (sgd_GetWindowHeight() / 2.0f));

	while((sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) == 0) {

		if (sgd_IsKeyHit(SGD_KEY_ESCAPE)) { break; }

		if (sgd_IsKeyDown(SGD_KEY_W)) {
			sgd_MoveEntity(camera, 0.0f, 0.0f, 0.15f);
		}

		if (sgd_IsKeyDown(SGD_KEY_S)) {
			sgd_MoveEntity(camera, 0.0f, 0.0f, -0.15f);
		}

		if (sgd_IsKeyDown(SGD_KEY_A)) {
			sgd_MoveEntity(camera, -0.15f, 0.0f, 0.0f);
		}

		if (sgd_IsKeyDown(SGD_KEY_D)) {
			sgd_MoveEntity(camera, 0.15f, 0.0f, 0.0f);
		}

		if (sgd_IsKeyDown(SGD_KEY_Q)) {
			sgd_TurnEntity(camera, 0.0f, 0.4f, 0.0f);
		} else {
			if (sgd_IsKeyDown(SGD_KEY_E)) {
				sgd_TurnEntity(camera, 0.0f, -0.4f, 0.0f);
			}
		}

		sgd_RenderScene();
		sgd_Present();
	}

	sgd_Terminate();

	return 0;
}
