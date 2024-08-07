#include "start.cpp"

void entry() {

	// Height of eifeel tower!
	float size = 330.0f;

	MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://misc/grass1K.jpg")).result();
	MeshPtr groundMesh = createBoxMesh({{-size, 0, -size}, {size, 0, size}}, groundMaterial);
	transformTexCoords(groundMesh, {size / 2, size / 2}, {0, 0});

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

	auto& config = scene->sceneRenderer()->sceneBindings()->lockConfigUniforms();
	config.csmTextureSize = 4096;
	config.csmClipRange = 660.0f;
	config.csmDepthBias = .0001f;
	scene->sceneRenderer()->sceneBindings()->unlockConfigUniforms();

//	FogEffectPtr fog = new FogEffect();
//	scene->sceneRenderer()->add(fog);

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
	setConfigVar("dawn.backendType", "Vulkan");
	setConfigVar("dawn.presentMode", "Fifo");
	setConfigVar("render.timeStampsEnabled", "1");
	start(entry);
}
