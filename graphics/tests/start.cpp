#include <graphics/exports.h>

#include <thread>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;
ScenePtr scene;
SkyboxPtr skybox;
LightPtr light;

void render() {
	scene->render();
	gc->present(gc->colorBuffer());
}

void start(void(*entry)()) {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [](CVec2u) {
		if (scene) render();
	});

	gc = new GraphicsContext(window, wgpu::BackendType::D3D12);

	scene = new Scene(gc);
	scene->ambientLightColor = {1, 1, 1, 0};

	auto skyTexture = loadTexture(Path("sgd://envmaps/grimmnight-cube.jpg"), TextureFormat::srgba8,
								  TextureFlags::cube | TextureFlags::mipmap | TextureFlags::filter)
						  .result();

	scene->envTexture = skyTexture;

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = skyTexture;
	scene->add(skybox);

	LightPtr light = new Light(LightType::directional);
	turn(light, {-halfPi / 2, 0, 0});
	scene->add(light);

	entry();
}
