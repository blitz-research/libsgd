#include "sgd/sgd.h"

int main()
{
	sgd_Init();
	sgd_CreateWindow(640,480,"Test",0);
	SGD_Light sun = sgd_CreateDirectionalLight();
	sgd_TurnEntity(sun,-50,-20,0);
	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera,0,1,0);
	SGD_Material ground_material = sgd_LoadPBRMaterial("sgd://materials/PavingStones065_1K-JPG");
	SGD_Mesh ground_mesh = sgd_CreateBoxMesh(-32,-0.1,-32,32,0,32,ground_material);
	SGD_Model ground = sgd_CreateModel(ground_mesh);
	int loop = 1;
	while (loop)
	{
		int e = sgd_PollEvents();
		if (e==SGD_EVENT_MASK_CLOSE_CLICKED) loop = 0;
		if (sgd_IsKeyDown(SGD_KEY_ESCAPE)) loop = 0;
		sgd_RenderScene();
		sgd_Present();
	}
	sgd_Terminate();
}
