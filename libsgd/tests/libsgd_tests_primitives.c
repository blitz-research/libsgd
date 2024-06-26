#include "start.c"

void entry() {

	SGD_Real tz = 0;

start:;	// Need semi-colon for Macos?!?

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 2, tz - 5.0);

	sgd_SetMaxPSMLights(5);
	sgd_SetPSMTextureSize(4096); // Nice high res shadow maps!
	sgd_SetPSMDepthBias(.0001f);

	SGD_Light light0 = sgd_CreatePointLight();
	sgd_SetLightShadowMappingEnabled(light0, SGD_TRUE);
	sgd_MoveEntity(light0, 0, 5, 0);
	sgd_SetLightRange(light0, 10);

	SGD_Material material = sgd_CreatePBRMaterial();
	sgd_SetMaterialVector4f(material, "albedoColor4f", 1, .5f, 0, 1);

	SGD_Model ground = sgd_CreateModel(0);
	sgd_SetModelMesh(ground, sgd_CreateBoxMesh(-10, -1, -20, 10, 0, 20, material));

	material = sgd_LoadPBRMaterial("sgd://misc/test-texture.png");
	sgd_SetMaterialFloat(material, "roughnessFactor1f", .5f);

	float r = .5f;
	float y = 1;

	SGD_Model models[5];

	SGD_Mesh mesh0 = sgd_CreateSphereMesh(r, 96, 48, material);
	sgd_SetMeshShadowCastingEnabled(mesh0, SGD_TRUE);
	models[0] = sgd_CreateModel(mesh0);
	sgd_MoveEntity(models[0], -2.5f, y, tz);

	float r2 = r * .7071f;
	SGD_Mesh mesh1 = sgd_CreateBoxMesh(-r2, -r2, -r2, r2, r2, r2, material);
	sgd_SetMeshShadowCastingEnabled(mesh1, SGD_TRUE);
	models[1] = sgd_CreateModel(mesh1);
	sgd_MoveEntity(models[1], -1.25f, r2, tz);

	SGD_Mesh mesh2 = sgd_CreateCylinderMesh(r * 2, r / 2, 96, material);
	sgd_SetMeshShadowCastingEnabled(mesh2, SGD_TRUE);
	models[2] = sgd_CreateModel(mesh2);
	sgd_MoveEntity(models[2], 0, r, tz);

	SGD_Mesh mesh3 = sgd_CreateConeMesh(r * 2, r / 2, 96, material);
	sgd_SetMeshShadowCastingEnabled(mesh3, SGD_TRUE);
	models[3] = sgd_CreateModel(mesh3);
	sgd_MoveEntity(models[3], 1.25f, r, tz);

	SGD_Mesh mesh4 = sgd_CreateTorusMesh(r * .75f, r * .25f, 96, 48, material);
	sgd_SetMeshShadowCastingEnabled(mesh4, SGD_TRUE);
	models[4] = sgd_CreateModel(mesh4);
	sgd_MoveEntity(models[4], 2.5f, y, tz);

	while (!(sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED)) {

		if (sgd_IsKeyDown(SGD_KEY_A)) {
			sgd_MoveEntity(camera, 0, 0, .1);
		} else if (sgd_IsKeyDown(SGD_KEY_Z)) {
			sgd_MoveEntity(camera, 0, 0, -.1);
		}

		if (sgd_IsKeyDown(SGD_KEY_LEFT)) {
			sgd_RotateEntity(camera, 0, 3, 0);
		} else if (sgd_IsKeyDown(SGD_KEY_RIGHT)) {
			sgd_RotateEntity(camera, 0, -3, 0);
		}

		if (sgd_IsKeyDown(SGD_KEY_UP)) {
			sgd_TurnEntity(camera, -3, 0, 0);
		} else if (sgd_IsKeyDown(SGD_KEY_DOWN)) {
			sgd_TurnEntity(camera, 3, 0, 0);
		}

		sgd_Clear2D();
		for (int i = 0; i < 5; ++i) {
			if(i!=1 && i!=2 && i!=3) sgd_TurnEntity(models[i], i / 10.0f, i / 5.0f + .1f, 0);
			if(sgd_CameraProject(camera, sgd_GetEntityX(models[i]), sgd_GetEntityY(models[i]), sgd_GetEntityZ(models[i]))) {
				sgd_Draw2DText("MODEL!", sgd_GetProjectedX(), sgd_GetProjectedY());
			}
		}

		{
			char buf[80];
			sprintf(buf, "FPS: %f, RPS: %f", sgd_GetFPS(), sgd_GetRPS());
			sgd_Draw2DText(buf, 2, 2);
		}

//		sgd_Clear2D();
		if (sgd_CameraPick(camera, sgd_GetMouseX(), sgd_GetMouseY(), ~0)) {
			sgd_Draw2DText("Picked!", 0, 20);
		}

		sgd_RenderScene();
		sgd_Present();
	}

	exit(0);
}

int main() {
	start(entry);
}
