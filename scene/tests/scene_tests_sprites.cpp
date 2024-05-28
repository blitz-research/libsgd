#include "start.cpp"

void entry() {

	EntityPtr pivot = new Model();
	scene->add(pivot);

	CameraPtr camera = new Camera(CameraType::perspective);
	camera->setParent(pivot);
	scene->add(camera);

	ImagePtr image = loadImage(Path("sgd://misc/grass1.png"), 1).result();
	image->spriteViewMode = SpriteViewMode::upright;
	image->spriteRect = Rectf{-1, 0, 1, 1};

	for (int i = 0; i < 20000; ++i) {

		SpritePtr sprite = new Sprite();
		sprite->image = image;
		scene->add(sprite);

		turn(sprite, {0, rnd(360), 0});
		move(sprite, {0, -1.5, rnd(100)});
	}

	auto gunMesh = createBoxMesh({{-1,-1,-1},{1,1,10}}, new Material(&pbrMaterialDescriptor));
	auto gunModel  = new Model();
	gunModel->mesh = gunMesh;
	scene->add(gunModel);

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	auto dc = overlay->drawList();
	dc->font = loadFont(Path("C:/windows/fonts/consola.ttf"),16).result();

	for (;;) {
		pollEvents();

		fly(pivot);

		aim(gunModel, pivot->worldPosition(), 0);

		static float zr;
		if (window->keyboard()->key(KeyCode::LEFT).down()) {
			zr += (-15 - zr) * .1f;
		} else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
			zr += (15 - zr) * .1f;
		} else {
			zr *= .9f;
		}
		setRotation(camera, {0, 0, zr});

		dc->clear();
		auto mode = image->spriteViewMode();
		dc->addText("SpriteViewMode: " + std::to_string((int)mode), {0,0} );
		if(window->keyboard()->key(KeyCode::SPACE).hit()) {
			image->spriteViewMode = (mode==SpriteViewMode::upright) ? SpriteViewMode::fixed : (SpriteViewMode)((int)mode+1);
		}

		render();
	}
}

int main() {
	start(entry);
}
