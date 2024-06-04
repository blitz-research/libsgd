#include "start.cpp"

void entry() {

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	DrawListPtr dc = overlay->drawList();

	float fh = 24;
	FontPtr font = loadFont(Path("sgd://fonts/FiraCode-Medium.ttf"), fh).result();
	dc->font = font;
	ImagePtr image = loadImage(Path("sgd://misc/explode64_vstrip.png"), 16).result();

	auto w = (float)window->size().x;
	auto h = (float)window->size().y;

	dc->fillColor = Vec4f(1, 0, 0, 1);
//	dc->addRect({0, 0, w, h});

	dc->fillColor = Vec4f(0, 1, 1, 1);
	dc->addLine({1, 1}, {w - 2, 1});
	dc->addLine({w - 2, 1}, {w - 2, h - 2});
	dc->addLine({w - 2, h - 2}, {1, h - 2});
	dc->addLine({1, h - 2}, {1, 1});

	dc->fillColor = Vec4f(0, 0, 0, 1);
//	dc->addRect({2, 2, w - 2, h - 2});

	dc->fillColor = Vec4f(0, 0, 0, 1);
	dc->addText("Hello World!", {2, 2});
	dc->addText("Hello world!", {2, fh + 2});
	dc->addText("Hello world!", {2, h - fh * 2 - 2});
	dc->addText("Hello world!", {2, h - fh - 2});

//	dc->clear();
	dc->pushLayer();

	float frame = 0;
	for (;;) {
		pollEvents();

		dc->clear();

		dc->addImage(image, {64,64}, frame+=.1f);

		auto str = std::to_string(gc->FPS());
		dc->addText(str,{w - font->textWidth(str),0});

		dc->addText(std::to_string(window->mouse()->position().z) + " " + std::to_string(window->mouse()->velocity().z), {0,64});

		render();
	}

	dc->popLayer();
}

int main() {
	start(entry);
}
