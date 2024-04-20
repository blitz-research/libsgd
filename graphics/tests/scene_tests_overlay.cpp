#include "start.cpp"

void entry() {

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	DrawListPtr dc = overlay->drawList();

	FontPtr font = loadFont(Path("sgd://fonts/FiraCode-Medium.ttf"), 16).result();
	dc->font = font;

	dc->addText("Hello World!", {4, 4});

//	window->mouse()->cursorMode = CursorMode::disabled;

	// dc->lineWidth = 3;
	// dc->pointSize = 3;

	dc->outlineEnabled = true;
	dc->outlineWidth=0;

	for (int i = 0; i < 1000; ++i) {
		dc->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		float w = rnd(32, 64);
		float h = rnd(32, 64);
		float x = rnd(1920 - w);
		float y = rnd(1080 - h);
		dc->addLine({rnd(1920),rnd(1080)},{rnd(1920),rnd(1080)});
//		dc->addPoint({x,y});
//		dc->addRect({x, y, x + w, y + h});
//		dc->addOval({x, y, x + w, y + h});
	}

	for (;;) {
		pollEvents();
		render();
	}
}

int main() {
	start(entry);
}
