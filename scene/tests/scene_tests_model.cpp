#include "start.cpp"

void entry() {

	// Height of eifeel tower!
	float size = 330.0f;

	MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://materials/Ground037_1K-JPG")).result();
	groundMaterial->setTexture("roughnessTexture", whiteTexture());
	groundMaterial->setFloat("roughnessFactor1f", .75f);
	MeshPtr groundMesh = createBoxMesh({{-size, 0, -size}, {size, 0, size}}, groundMaterial);
	transformTexCoords(groundMesh, {size, size}, {0, 0});

	MeshPtr towerMesh = loadStaticMesh(Path("sgd://models/eiffel_tower.glb")).result();
	fit(towerMesh, {{-size / 2, 0, -size / 2}, {size / 2, size, size / 2}}, true);
	towerMesh->shadowsEnabled = true;

	for (int ix = 0; ix <= 0; ++ix) {
		for (int iz = 0; iz <= 0; ++iz) {
			ModelPtr groundModel = new Model(groundMesh);
			ModelPtr towerModel = new Model(towerMesh);
			towerModel->setParent(groundModel);
			move(groundModel, {(float)ix * size * 2, 0, (float)iz * size * 2});
			scene->add(groundModel);
		}
	}

	createPlayer(nullptr);
	move(player, {0, 10, -100});

	light->setWorldPosition({0, 0, 0});
	light->shadowsEnabled = true;
	setRotation(light, {-30, 0, 0});

#if 0
	camera->near = .125f;
	camera->far = 1024;
	scene->sceneBindings()->csmSplitDistances = {16, 64, 256, 1024};
	scene->sceneBindings()->csmTextureSize = 2048;
	scene->sceneBindings()->csmDepthBias = .0001f;
	scene->sceneBindings()->csmClipRange = 330.0f;
#endif

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(1.25f);

		light->setWorldPosition({});

		rotate(light, {0, .025f, 0});

		dc->clear();
		dc->addText("FPS:" + std::to_string(currentGC()->FPS()), {0, 0});
		dc->addText("RPS:" + std::to_string(scene->sceneRenderer()->RPS()), {0, 16});

		render();
	}
}

int main() {
	setConfigVar("dawn.backendType","Vulkan");
	setConfigVar("dawn.presentMode","Mailbox");
	start(entry);
}
