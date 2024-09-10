#include "start.cpp"

void entry() {

	DrawListPtr dc = overlay->drawList();

	ImagePtr image = loadImage(Path("sgd://misc/explode64_vstrip.png"), 16).result();

	auto w = (float)window->size().x;
	auto h = (float)window->size().y;

	dc->outlineEnabled = false;
	for (int i = 0; i < 25; ++i) {
		// dc->lineWidth = rnd(1, 7);
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

	MaterialPtr mat = loadPrelitMaterial(Path("sgd://misc/StillLife.exr")).result();
	mat->blendMode = BlendMode::alphaBlend;

	dc->fillColor = {1.0, 0.75f, 0, 1};
	dc->fillMaterial = mat;

	float frame = 0;
	for (;;) {
		pollEvents();

		dc->clear();

		dc->addRect({0, 0, (float)window->size().x, (float)window->size().y});

		dc->addImage(image, Vec2f(window->mouse()->position()), frame += .1f);

		auto str = std::to_string(currentGC()->FPS());
		dc->addText(str, {w - dc->font()->textWidth(str), 0});

		dc->addText(std::to_string(window->mouse()->position().z) + " " + std::to_string(window->mouse()->velocity().z),
					{0, 64});

		render();
	}

	dc->popLayer();
}

int main() {
	start(entry);
}
