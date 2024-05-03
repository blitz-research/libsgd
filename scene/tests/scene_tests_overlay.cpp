#include "start.cpp"

void entry() {

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	DrawListPtr dc = overlay->drawList();

	float fh = 32;
	FontPtr font = loadFont(Path("C:/windows/fonts/consola.ttf"), fh).result();
	dc->font = font;

	dc->addText("Hello World!", {4, 4});

	//	window->mouse()->cursorMode = CursorMode::disabled;

	// dc->lineWidth = 3;
	// dc->pointSize = 3;

	dc->outlineEnabled = true;
	dc->outlineWidth = 0;

	for (int i = 0; i < 1000; ++i) {
		dc->fillColor = Vec4f(rnd(), rnd(), rnd(), 1);
		float w = rnd(32, 64);
		float h = rnd(32, 64);
		float x = rnd(1920 - w);
		float y = rnd(1080 - h);
		dc->addLine({rnd(1920), rnd(1080)}, {rnd(1920), rnd(1080)});
		//		dc->addPoint({x,y});
		//		dc->addRect({x, y, x + w, y + h});
		//		dc->addOval({x, y, x + w, y + h});
	}

	dc->clear();

	dc->fillColor = Vec4f(1, 0, 0, 1);


	auto w = (float)window->size().x;
	auto h = (float)window->size().y;

	dc->fillColor = Vec4f(1, 0, 0, 1);
	dc->addRect({0, 0, (float)window->size().x, (float)window->size().y});

	dc->fillColor = Vec4f(0, 0, 1, 1);
	dc->addLine({1, 1}, {w - 2, 1});
	dc->addLine({w - 2, 1}, {w - 2, h - 2});
	dc->addLine({w - 2, h - 2}, {1, h - 2});
	dc->addLine({1, h - 2}, {1, 1});

	dc->fillColor = Vec4f(0, 0, 0, 1);
	dc->addRect({2, 2, w - 2, h - 2});

	dc->fillColor = Vec4f(0, 0, 0, 1);
	dc->addText("Hello world!", {0, 0});
	dc->addText("Hello world!", {0, fh});
	dc->addText("Hello world!", {0, h - fh*2});
	dc->addText("Hello world!", {0, h - fh});

	auto material = loadPrelitMaterial(Path("sgd://misc/test-texture.png")).result();
	material->blendMode = BlendMode::alpha;
	material->depthFunc = DepthFunc::always;
	material->cullMode = CullMode::none;

//	auto material = new Material(&prelitMaterialDescriptor);
//	material->setVector4f("albedoColor4f", Vec4(1,0,0,1));

	dc->fillMaterial = material;
	dc->fillColor = Vec4f(1);
	dc->addRect({0,0,256,256});

	for (;;) {
		pollEvents();
		render();
	}
}

int main() {
	start(entry);
}
