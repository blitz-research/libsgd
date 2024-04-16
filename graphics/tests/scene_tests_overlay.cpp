#include "start.cpp"

void entry() {

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	DrawListPtr dc = overlay->drawList();

	FontPtr font = loadFont(Path("sgd://fonts/FiraCode-Medium.ttf"), 16).result();

	dc->font = font;

	dc->addText("Hello World!", 4, 4);

	overlay->drawList()->lineWidth = 7;

	for (int i = 0; i < 1000; ++i) {
		overlay->drawList()->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		overlay->drawList()->addLine(rnd(1920), rnd(1080), rnd(1920), rnd(1080));
	}

	for (;;) {
//		dc->clear();
//		dc->addText("FPS:" + std::to_string(gc->FPS()), 0, 0);

		pollEvents();
		render();
	}
}

int main() {
	start(entry);
}
