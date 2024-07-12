#include "start.cpp"

void entry() {

	DrawListPtr dc = overlay->drawList();

//	ImagePtr image = loadImage(Path("sgd://misc/explode64_vstrip.png"), 16).result();

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

	auto fh = dc->font()->height;

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

//		dc->addImage(image, {64,64}, frame+=.1f);

		auto str = std::to_string(currentGC()->FPS());
		dc->addText(str,{w - dc->font()->textWidth(str),0});

		dc->addText(std::to_string(window->mouse()->position().z) + " " + std::to_string(window->mouse()->velocity().z), {0,64});

		render();
	}

	dc->popLayer();
}

int main() {

	setConfigVar("dawn.backendType","D3D12");

	start(entry);
}
