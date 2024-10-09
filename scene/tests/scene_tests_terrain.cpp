#include "start.cpp"

void entry() {

	SGD_LOG << "Loading material...";
	MaterialPtr material = new Material(&pbrMaterialDescriptor);
	TexturePtr albedoTexture = load2DTexture(Path("sgd://terrains/canyon/albedo.png"), TextureFormat::any, TextureFlags::image).result();
	material->setTexture("albedo",albedoTexture);

	SGD_LOG << "Loading height texture...";
	TexturePtr heightTexture =
		load2DTexture(Path("sgd://terrains/canyon/height.exr"), TextureFormat::any, TextureFlags::default_).result();
//		load2DTexture(Path("~/Desktop/height_test.png"), TextureFormat::any, TextureFlags::image).result();

	SGD_LOG << "Loading normal texture...";
	TexturePtr normalTexture =
		load2DTexture(Path("sgd://terrains/canyon/normal.png"), TextureFormat::rgba8, TextureFlags::default_).result();
//		load2DTexture(Path("~/Desktop//normal_test.png"), TextureFormat::rgba8, TextureFlags::image).result();

	TerrainPtr terrain = new Terrain();
	scene->add(terrain);

	terrain->bindings()->size = 4096;
	terrain->bindings()->lods = 5;
	terrain->bindings()->material = material;
	terrain->bindings()->materialSize = 4096;
	terrain->bindings()->debugMode = 0;
	terrain->bindings()->heightTexture = heightTexture;

//	terrain->bindings()->normalTexture = normalTexture;
	//terrain->bindings()->shadowsEnabled = true;

	//light->shadowsEnabled = true;
//	setConfigVar("csm.depthBias","0.001");
//	scene->sceneRenderer()->sceneBindings()->lockConfigUniforms() = getConfigUniformsFromConfigVars();
//	scene->sceneRenderer()->sceneBindings()->unlockConfigUniforms();

	setScale(terrain, {1, 512, 1});

	createPlayer(nullptr);
	move(player, {0, 512, 88});

	camera->near= 1;
	camera->far = 3072;

	DrawListPtr dc = overlay->drawList();

	for (;;) {
		pollEvents();

		if(window->keyboard()->key(KeyCode::SPACE).hit()) {
			terrain->bindings()->debugMode = !terrain->bindings()->debugMode();
		}

		playerFly(.25f);

		dc->clear();
		dc->addText(String("Camera: ") + toString(camera->worldPosition()), {0, 0});
		dc->addText(String("FPS: ") + toString(currentGC()->FPS()), {0, 16});

		render();
	}
}

int main() {
	start(entry);
}
