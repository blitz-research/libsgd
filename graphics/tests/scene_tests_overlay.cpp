#include "start.cpp"

void entry() {

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	DrawListPtr dc = overlay->drawList();

	FontPtr font = loadFont(Path("sgd://fonts/FiraCode-Medium.ttf"), 16).result();
	dc->font = font;

	dc->addText("Hello World!", {4, 4});

	window->mouse()->cursorMode = CursorMode::disabled;

	// dc->lineWidth = 3;
	// dc->pointSize = 3;

	dc->outlineEnabled = true;
	dc->outlineWidth=3;

	for (int i = 0; i < 1000; ++i) {
		dc->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		float w = rnd(32, 64);
		float h = rnd(32, 64);
		float x = rnd(1920 - w);
		float y = rnd(1080 - h);
		dc->addOval({x, y, x + w, y + h});
		// dc->addPoint({std::floor(rnd(1920)), std::floor(rnd(1080))});
		// dc->addPoint(std::floor(rnd(1920)) + .5f, std::floor(rnd(1080)) + .5f);
		//		overlay->drawList()->addLine(rnd(1920), rnd(1080), rnd(1920), rnd(1080));
	}

	for (;;) {
		//		dc->clear();
		//		dc->addText("FPS:" + std::to_string(gc->FPS()), 0, 0);

		dc->clear();
		dc->addText("Position: " + toString(window->mouse()->position()) + " Velocity: " + toString(window->mouse()->velocity()), {0,0});

		pollEvents();
		render();
	}
}

int main() {
	start(entry);
}
