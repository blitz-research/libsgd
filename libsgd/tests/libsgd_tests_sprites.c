#include "start.c"

void entry() {

	sgd_CreatePerspectiveCamera();

	SGD_Light light = sgd_CreateDirectionalLight();
	sgd_TurnEntity(light, 0,-45,0);

	SGD_Image image = sgd_LoadImage("sgd://misc/スノー.png", SGD_TEXTURE_FORMAT_SRGBA8, SGD_TEXTURE_FLAGS_IMAGE_DEFAULT, 1);

	int n = 10000;
	for (int i = 0; i < n; ++i) {
		SGD_Sprite sprite = sgd_CreateSprite();
		sgd_SetSpriteImage(sprite, image);
		sgd_TurnEntity(sprite, 0, (float)rand() * 360 / RAND_MAX, 0); 		//NOLINT
		sgd_MoveEntity(sprite, 0, -1.5f, (float)rand() * 50 / RAND_MAX);	//NOLINT
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
