#include "start.cpp"

void entry() {

	createPlayer(nullptr);
	move(player,{0,1.5f,0});

	ImagePtr image = loadImage(Path("sgd://misc/grass1.png")).result();
	image->viewMode = ImageViewMode::fixed;
	image->rect = Rectf(-.5f,0,.5f,.5f);

	for (int i = 0; i < 10000; ++i) {

		SpritePtr sprite = new Sprite(image);
		scene->add(sprite);

		turn(sprite, {0, rnd(360), 0});
		move(sprite, {0, 0, rnd(100)});

		auto sc = rnd(1,5);
		setScale(sprite,{sc,sc,sc});
	}

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(.5f);

		dc->clear();
		auto mode = image->viewMode();
		dc->addText("ImageViewMode: " + std::to_string((int)mode), {0, 0});
		if (window->keyboard()->key(KeyCode::SPACE).hit()) {
			image->viewMode = (mode == ImageViewMode::upright) ? ImageViewMode::fixed : (ImageViewMode)((int)mode + 1);
		}

		dc->addText("FPS:" + toString(currentGC()->FPS()), {0, 16});
		dc->addText("RPS:" + toString(scene->sceneRenderer()->RPS()), {0, 32});

		render();
	}
}

int main() {
	//setConfigVar("dawn.backendType", "Vulkan");
	start(entry);
}
