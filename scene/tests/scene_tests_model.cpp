#include "start.cpp"

void entry() {

	float groundSize = 500.0f;
	MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://materials/Ground037_1K-JPG")).result();
	groundMaterial->setTexture("roughnessTexture", whiteTexture());
	groundMaterial->setFloat("roughnessFactor1f", .75f);
	MeshPtr groundMesh =
		createBoxMesh({{-groundSize / 2, -1, -groundSize / 2}, {groundSize / 2, 0, groundSize / 2}}, groundMaterial);
	transformTexCoords(groundMesh, {groundSize, groundSize}, {0, 0});

	float towerSize = 330.0f;
	MeshPtr towerMesh = loadStaticMesh(Path("sgd://models/eiffel_tower.glb")).result();
	fit(towerMesh, {{-towerSize, 0, -towerSize}, {towerSize / 2, towerSize, towerSize / 2}}, true);
	towerMesh->shadowsEnabled = true;

	for (int ix = -2; ix <= 2; ++ix) {
		for (int iz = -2; iz <= 2; ++iz) {
			ModelPtr groundModel = new Model(groundMesh);
			ModelPtr towerModel = new Model(towerMesh);
			towerModel->setParent(groundModel);
			move(groundModel, {(float)ix * groundSize, 0, (float)iz * groundSize});
			scene->add(groundModel);
		}
	}

	createPlayer(nullptr);
	move(player, {0, 1, -2});

	light->setWorldPosition({0, 0, 0});
	light->shadowsEnabled = true;
	setRotation(light, {-30, 0, 0});

	camera->near = .125f;
	camera->far = 8192;
	scene->sceneBindings()->csmSplitDistances = {128, 512, 2048, 8192};
	scene->sceneBindings()->csmTextureSize = 4096;
	scene->sceneBindings()->csmDepthBias = .0001f;

#if 0
	camera->near = .125f;
	camera->far = 1024;
	scene->sceneBindings()->csmSplitDistances={16, 64, 256, 1024};
	scene->sceneBindings()->csmTextureSize = 4096;
	scene->sceneBindings()->csmDepthBias = .0001f;
#endif

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
