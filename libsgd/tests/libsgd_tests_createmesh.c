#include "start.c"

SGD_Mesh createSphereMesh(float radius, int xSegs, int ySegs, SGD_Material material) {

	SGD_Mesh mesh = sgd_CreateMesh(0,0);

	float fxSegs = 1 / (float)xSegs, fySegs = 1 / (float)ySegs;

	for (int i = 0; i < xSegs; ++i) {
		sgd_AddMeshVertex(mesh, 0, radius, 0, 0, 1, 0, ((float)i + .5f) * 2 * fxSegs, 0);
	}
	for (int j = 1; j < ySegs; ++j) {
		float pitch = SGD_HALF_PI - (float)j * SGD_PI * fySegs;
		for (int i = 0; i <= xSegs; ++i) {
			float yaw = (float)i * SGD_TWO_PI * fxSegs;
			float r = cosf(pitch);
			float y = sinf(pitch);
			float x = cosf(yaw) * r;
			float z = sinf(yaw) * r;
			sgd_AddMeshVertex(mesh, x * radius, y * radius, z * radius, x, y, z, (float)i * 2 * fxSegs, (float)j * fySegs);
		}
	}
	for (int i = 0; i < xSegs; ++i) {
		sgd_AddMeshVertex(mesh, 0, -radius, 0, 0, -1, 0, ((float)i + .5f) * 2 * fxSegs, 1);
	}

	SGD_Surface surf = sgd_CreateSurface(mesh, 0, material);

	for (int i = 0; i < xSegs; ++i) {
		sgd_AddSurfaceTriangle(surf, i, i + xSegs, i + xSegs + 1);
	}
	for (int j = 1; j < ySegs - 1; ++j) {
		for (int i = 0; i < xSegs; ++i) {
			int v0 = j * (xSegs + 1) + i - 1;
			int v2 = v0 + xSegs + 2;
			sgd_AddSurfaceTriangle(surf, v0, v2, v0 + 1);
			sgd_AddSurfaceTriangle(surf, v0, v2 - 1, v2);
		}
	}
	for (int i = 0; i < xSegs; ++i) {
		int v0 = (xSegs + 1) * (ySegs - 1) + i - 1;
		sgd_AddSurfaceTriangle(surf, v0, v0 + xSegs + 1, v0 + 1);
	}

	return mesh;
}

void entry() {

	SGD_Camera camera = sgd_CreatePerspectiveCamera();
	sgd_MoveEntity(camera, 0, 0, -5);

	SGD_Light light0 = sgd_CreatePointLight();
	sgd_MoveEntity(light0, 0, 5, -5);
	sgd_SetEntityParent(light0, camera);
	sgd_SetLightRange(light0, 50);

	SGD_Material material = sgd_LoadPBRMaterial("sgd://misc/test-texture.png");
	sgd_SetMaterialFloat(material, "roughnessFactor1f", .5f);

//	SGD_Mesh mesh = sgd_CreateSphereMesh(1, 96, 48, material);

	SGD_Mesh mesh = createSphereMesh(1, 96, 48, material);

	SGD_Model model = sgd_CreateModel(mesh);

	while (!(sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED)) {

//		sgd_TurnEntity(model, 0, .7, 0);

		sgd_RenderScene();
		sgd_Present();
	}
}

int main() {
	start(entry);
}
