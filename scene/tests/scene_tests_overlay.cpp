#include "start.cpp"

#if 0
void entry() {

	setConfigVar("gltf.loggingEnabled","1");

	camera = new Camera(CameraType::perspective);
	scene->add(camera);
	move(camera,{0,0,-2});

	MeshPtr mesh = loadStaticMesh(Path("sgd://models/helmet.glb")).result();

	ModelPtr model = new Model(mesh);
	scene->add(model);

	for (;;) {
		pollEvents();

		if(window->keyboard()->key(KeyCode::LEFT).down()) {
			turn(model, {0, 1, 0});
		}else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
			turn(model, {0, -1, 0});
		}
		if(window->keyboard()->key(KeyCode::UP).down()) {
			turn(model, {1, 0, 0});
		}else if (window->keyboard()->key(KeyCode::DOWN).down()) {
			turn(model, {-1, 0, 0});
		}

		render();
	}
}

#endif

#if 1
void entry() {

	DrawListPtr dc = overlay->drawList();

	ImagePtr image = loadImage(Path("sgd://misc/explode64_vstrip.png"), 16).result();

	auto w = (float)window->size().x;
	auto h = (float)window->size().y;

	dc->outlineEnabled = false;
	for (int i = 0; i < 25; ++i) {
		//dc->lineWidth = rnd(1, 7);
		dc->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		dc->addLine({rnd(w), rnd(h)}, {rnd(w), rnd(h)});
	}

	dc->outlineEnabled = true;
	for (int i = 0; i < 25; ++i) {
		dc->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		auto x = rnd(w), y = rnd(h);
		dc->addRect(Rectf(x, y, x + rnd(32, 64), y + rnd(32, 64)));
	}

	dc->outlineEnabled = true;
	for (int i = 0; i < 25; ++i) {
		dc->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		auto x = rnd(w), y = rnd(h);
		dc->addOval(Rectf(x, y, x + rnd(32, 64), y + rnd(32, 64)));
	}

	dc->pushLayer();

	float frame = 0;
	for (;;) {
		pollEvents();

		dc->clear();

		dc->addImage(image, Vec2f(window->mouse()->position()), frame += .1f);

		auto str = std::to_string(currentGC()->FPS());
		dc->addText(str, {w - dc->font()->textWidth(str), 0});

		dc->addText(std::to_string(window->mouse()->position().z) + " " + std::to_string(window->mouse()->velocity().z),
					{0, 64});

		render();
	}

	dc->popLayer();
}
#endif

int main() {

	setConfigVar("dawn.backendType", "D3D12");

	start(entry);
}
