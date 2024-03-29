#include "start.c"

void entry() {

	SGD_Light light = sgd_CreateDirectionalLight();
	sgd_TurnEntity(light, 0,-45,0);

	SGD_Material material = sgd_LoadSpriteMaterial("sgd://misc/grass1.png");
	sgd_SetMaterialBlendMode(material, SGD_BLEND_MODE_ALPHA);

	int n = 10000;
	for (int i = 0; i < n; ++i) {
		SGD_Sprite sprite = sgd_CreateSprite();
		sgd_SetSpriteMaterial(sprite, material);
		sgd_TurnEntity(sprite, 0, (float)rand() * 360 / RAND_MAX, 0);
		sgd_MoveEntity(sprite, 0, -1.5f, (float)rand() * 50 / RAND_MAX);
	}

	while( !(sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED)){
		sgd_RenderScene();
		sgd_Present();
	}

	exit(0);
}

int main() {
	start(entry);
}
