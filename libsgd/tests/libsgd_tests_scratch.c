#include "start.c"

void entry() {

	SGD_Image image = sgd_LoadImage("sgd://misc/light.png",1);

	SGD_Sprite sprite = sgd_CreateSprite(image);

	SGD_Camera camera = sgd_CreatePerspectiveCamera();

	sgd_MoveEntity( camera,0,0,-5);

	while(!(sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED)) {
		sgd_RenderScene();
		sgd_Present();
	}
}

int main() {
	start(entry);
}
