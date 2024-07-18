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

#if 0
	LightPtr headLight = new Light(LightType::spot);
	scene->add(headLight);
	move(headLight, {0,5,0});
	turn(headLight, {-90,0,0});
	headLight->shadowsEnabled = true;
	headLight->setParent(player);
	headLight->range = 120;
	headLight->falloff = 0;
	headLight->innerConeAngle = 15;
	headLight->outerConeAngle = 30;
#endif

	light->setWorldPosition({0, 0, 0});
	light->shadowsEnabled = true;
	setRotation(light, {-30, 0, 0});
	//	light->setIsVisible(false);

	auto& config = scene->sceneRenderer()->sceneBindings()->lockConfigUniforms();
	config.csmTextureSize = 4096;
	config.csmClipRange = 660.0f;
	config.csmDepthBias = .0001f;
	scene->sceneRenderer()->sceneBindings()->unlockConfigUniforms();

	scene->sceneRenderer()->ambientLightColor = Vec4f(0, 0, 0, 0);

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
	setConfigVar("dawn.backendType", "Vulkan");
	setConfigVar("dawn.presentMode", "Fifo");
	setConfigVar("render.timeStampsEnabled", "1");
	start(entry);
}
