#include "start.cpp"

void entry() {

	float sz = 1024;
	MeshPtr groundMesh = loadStaticMesh(Path("~/dev/assets/landscape_free.glb")).result();
	fit(groundMesh, {{-sz / 2, 0, -sz / 2}, {sz / 2, sz, sz / 2}}, true);
	groundMesh->shadowsEnabled = true;
	ModelPtr groundModel = new Model(groundMesh);
	scene->add(groundModel);

	createPlayer(nullptr);
	move(player, {0, sz / 2, -2});

	light->setWorldPosition({0, 0, 0});
	setRotation(light, {-30, 0, 0});

	light->shadowsEnabled = true;

	camera->near = .125f;
	camera->far = 1024;
	scene->sceneBindings()->csmSplitDistances = {16, 64, 256, 1024};
	scene->sceneBindings()->csmTextureSize = 2048;
	scene->sceneBindings()->csmDepthBias = .0001f;

	auto overlay = new Overlay();
	scene->add(overlay);

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(1);

		light->setWorldPosition({});

		rotate(light, {0, .025f, 0});

		dc->clear();
		dc->addText("FPS:" + std::to_string(gc->FPS()), {0, 0});
		dc->addText("RPS:" + std::to_string(scene->RPS()), {0, 16});

		render();
	}
}

int main() {
	start(entry);
}
