#include "start.cpp"

void entry() {

	SGD_LOG << "Loading material...";
	MaterialPtr material = loadPBRMaterial(Path("~/Desktop/Canyon/Canyon Diffuse.png")).result();

	SGD_LOG << "Loading height texture...";
	TexturePtr heightTexture =
		load2DTexture(Path("~/Desktop/Canyon/Canyon Height Map EXR.exr"), TextureFormat::any, TextureFlags::default_).result();

	SGD_LOG << "Loading normal texture...";
	TexturePtr normalTexture =
		load2DTexture(Path("~/Desktop/Canyon/Canyon Normal Map.png"), TextureFormat::rgba8, TextureFlags::default_).result();

	TerrainPtr terrain = new Terrain();
	scene->add(terrain);

	terrain->bindings()->size = 3072;
	terrain->bindings()->lods = 4;
	terrain->bindings()->material = material;
	terrain->bindings()->materialSize = 4096;
	terrain->bindings()->debugMode = 0;
	terrain->bindings()->heightTexture = heightTexture;
	terrain->bindings()->normalTexture = normalTexture;
	terrain->bindings()->shadowsEnabled = true;//false;

	light->shadowsEnabled = true;

	setConfigVar("csm.depthBias","0.001");

	scene->sceneRenderer()->sceneBindings()->lockConfigUniforms() = getConfigUniformsFromConfigVars();
	scene->sceneRenderer()->sceneBindings()->unlockConfigUniforms();

	setScale(terrain, {1, 2048, 1});

	createPlayer(nullptr);
	move(player, {0, 512, 0});

	DrawListPtr dc = overlay->drawList();

	for (;;) {
		pollEvents();

		if(window->keyboard()->key(KeyCode::SPACE).hit()) {
			terrain->bindings()->debugMode = !terrain->bindings()->debugMode();
		}

		playerFly(.25f);

		dc->clear();
		dc->addText(String("Camera Y: ") + toString(camera->worldPosition().y), {0, 0});
		dc->addText(String("FPS: ") + toString(currentGC()->FPS()), {0, 16});

		render();
	}
}

int main() {
	start(entry);
}
