#include "start.cpp"

/*
void entry() {
	for (;;) {
		pollEvents();

		render();
	}
}

int main() {
	start(entry);
}
*/

void entry() {

	ImagePtr image = loadImage(Path("sgd://misc/light.png")).result();

	SpritePtr sprite = new Sprite(image);
	scene->add(sprite);

	camera = new Camera(CameraType::perspective);
	scene->add(camera);

	move(camera,{0,0,-5});

	for (;;) {

		pollEvents();

		render();
	}
}

int main() {
	start(entry);
}
