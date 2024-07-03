#include "start.cpp"

void entry() {

	createPlayer(nullptr);

	ImagePtr image = loadImage(Path("sgd://misc/grass1.png"), 1).result();
	image->spriteViewMode = SpriteViewMode::fixed;
	image->spriteRect = Rectf{-1, 0, 1, 1};

	for (int i = 0; i < 60000; ++i) {

		SpritePtr sprite = new Sprite();
		sprite->image = image;
		scene->add(sprite);

		turn(sprite, {0, rnd(360), 0});		// 59
		move(sprite, {0, -1.5, rnd(100)});	// 1017

		if(i==0){
			SGD_LOG << "Trans, Rot, Scale"<<sprite->worldPosition() << sprite->worldBasis().k << sprite->worldScale();
		}

		setScale(sprite, {.25f, .25f, .25f});

		if(i==0){
			SGD_LOG << "Trans, Rot, Scale"<<sprite->worldPosition() <<  sprite->worldBasis().k << sprite->worldScale();
		}
	}

	auto gunMesh = createBoxMesh({{-1,-1,-1},{1,1,10}}, new Material(&pbrMaterialDescriptor));
	auto gunModel  = new Model();
	gunModel->mesh = gunMesh;
	scene->add(gunModel);

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(.5f);

		aim(gunModel, camera->worldPosition(), 0);
/*
		static float zr;
		if (window->keyboard()->key(KeyCode::LEFT).down()) {
			zr += (-15 - zr) * .1f;
		} else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
			zr += (15 - zr) * .1f;
		} else {
			zr *= .9f;
		}
		setRotation(camera, {0, 0, zr});
		*/

		dc->clear();
		auto mode = image->spriteViewMode();
		dc->addText("SpriteViewMode: " + std::to_string((int)mode), {0,0} );
		if(window->keyboard()->key(KeyCode::SPACE).hit()) {
			image->spriteViewMode = (mode==SpriteViewMode::upright) ? SpriteViewMode::fixed : (SpriteViewMode)((int)mode+1);
		}

		dc->addText("FPS:"+toString(gc->FPS()),{0,16});
		dc->addText("RPS:"+toString(scene->RPS()),{0,32});

		render();
	}
}

int main() {
	start(entry);
}
