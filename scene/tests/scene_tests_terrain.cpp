#include "start.cpp"

void entry() {

	SGD_LOG << "Loading material...";
	TexturePtr albedoTexture = load2DTexture(Path("~/Desktop/canyon/albedo.png"), TextureFormat::any, TextureFlags::image).result();
	MaterialPtr material = new Material(&pbrMaterialDescriptor);
	material->setTexture("albedo",albedoTexture);

	SGD_LOG << "Loading height texture...";
	TexturePtr heightTexture =
		load2DTexture(Path("~/Desktop/canyon/height2.exr"), TextureFormat::any, TextureFlags::image).result();

	SGD_LOG << "Loading normal texture...";
	TexturePtr normalTexture =
		load2DTexture(Path("~/Desktop/canyon/normal.png"), TextureFormat::rgba8, TextureFlags::image).result();

	TerrainPtr terrain = new Terrain();
	scene->add(terrain);

	terrain->bindings()->size = 1024;
	terrain->bindings()->lods = 1;
	terrain->bindings()->material = material;
	terrain->bindings()->materialSize = 4096;
	terrain->bindings()->debugMode = 0;
	terrain->bindings()->heightTexture = heightTexture;
	terrain->bindings()->normalTexture = normalTexture;
	//terrain->bindings()->shadowsEnabled = true;

	//light->shadowsEnabled = true;


	setConfigVar("csm.depthBias","0.001");

	scene->sceneRenderer()->sceneBindings()->lockConfigUniforms() = getConfigUniformsFromConfigVars();
	scene->sceneRenderer()->sceneBindings()->unlockConfigUniforms();

	setScale(terrain, {1, 512, 1});

	createPlayer(nullptr);
	move(player, {0, 191, 88});

	camera->near= 1;
	camera->far = 4000;

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
