#include "start.c"

void entry() {

start:

	sgd_SetSceneAmbientLightColor(0, 0, 0, 1);

	//	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	//	sgd_TurnEntity(camera, -30, 0, 0);

	SGD_Light light0 = sgd_CreatePointLight();
//	sgd_MoveEntity(light0,0,5,0);
	sgd_SetLightRange(light0, 50);

	SGD_Material material = sgd_LoadPBRMaterial("sgd://misc/test-texture.png");
	sgd_SetMaterialFloat(material, "roughnessFactor1f", .5f);

	//SGD_Material material = sgd_LoadPBRMaterial("sgd://materials/Fabric050_1K-JPG");

	float r = .5f, z=3.5f;

	SGD_Mesh mesh0 = sgd_CreateSphereMesh(r, 96, 48, material);
	SGD_Model model0 = sgd_CreateModel();
	sgd_SetModelMesh(model0, mesh0);
	sgd_MoveEntity(model0, -2.5f, 0, z);

	float r2=r*.7071f;
	SGD_Mesh mesh1 = sgd_CreateBoxMesh(-r2, -r2, -r2, r2, r2, r2, material);
	SGD_Model model1 = sgd_CreateModel();
	sgd_SetModelMesh(model1, mesh1);
	sgd_MoveEntity(model1, -1.25f, 0, z);

	SGD_Mesh mesh2 = sgd_CreateCylinderMesh(r * 2, r / 2, 96, material);
	SGD_Model model2 = sgd_CreateModel();
	sgd_SetModelMesh(model2, mesh2);
	sgd_MoveEntity(model2, 0, 0, z);

	SGD_Mesh mesh3 = sgd_CreateConeMesh(r * 2, r / 2, 96, material);
	SGD_Model model3 = sgd_CreateModel();
	sgd_SetModelMesh(model3, mesh3);
	sgd_MoveEntity(model3, 1.25f, 0, z);

	SGD_Mesh mesh4 = sgd_CreateTorusMesh(r * .75f, r * .25f, 96, 48, material);
	SGD_Model model4 = sgd_CreateModel();
	sgd_SetModelMesh(model4, mesh4);
	sgd_MoveEntity(model4, 2.5f, 0, z);

	for (;;) {
		if (sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) break;

		if(sgd_KeyHit(SGD_KEY_ESCAPE)) {
			sgd_DestroyEntity(model0);
			sgd_DestroyEntity(model1);
			sgd_DestroyEntity(model2);
			sgd_DestroyEntity(model3);
			sgd_DestroyEntity(model4);
			reset();
			goto start;
		}

		sgd_TurnEntity(model0, .3, .7, 0);
		sgd_TurnEntity(model1, .3, .7, 0);
		sgd_TurnEntity(model2, .3, .7, 0);
		sgd_TurnEntity(model3, .3, .7, 0);
		sgd_TurnEntity(model4, .3, .7, 0);

		sgd_RenderScene();

		sgd_Present();
	}

	exit(0);
}

int main() {
	start(entry);
}
