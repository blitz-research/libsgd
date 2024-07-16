#include "start.cpp"

void entry() {

	createPlayer(nullptr);

	ImagePtr image = loadImage(Path("sgd://misc/grass1.png"), 1).result();
	image->viewMode = ImageViewMode::fixed;
	image->rect = Rectf{-1, 0, 1, 1};

	for (int i = 0; i < 50000; ++i) {

		SpritePtr sprite = new Sprite();
		sprite->image = image;
		scene->add(sprite);

		turn(sprite, {0, rnd(360), 0});
		move(sprite, {0, -1.5, rnd(100)});

		if(i==0){
			SGD_LOG << "Trans, Rot, Scale"<<sprite->worldPosition() << sprite->worldBasis().k << sprite->worldScale();
		}

		setScale(sprite, {.25f, .25f, .25f});

		if(i==0){
			SGD_LOG << "Trans, Rot, Scale"<<sprite->worldPosition() <<  sprite->worldBasis().k << sprite->worldScale();
		}
	}

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(.5f);

		dc->clear();
		auto mode = image->viewMode();
		dc->addText("ImageViewMode: " + std::to_string((int)mode), {0,0} );
		if(window->keyboard()->key(KeyCode::SPACE).hit()) {
			image->viewMode = (mode==ImageViewMode::upright) ? ImageViewMode::fixed : (ImageViewMode)((int)mode+1);
		}

		render();
	}
}

int main() {
	start(entry);
}
