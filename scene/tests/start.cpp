#include <scene/exports.h>

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

void fly(Entity* camera) {

	static float yr;
	if (window->keyboard()->key(KeyCode::LEFT).down()) {
		yr += (1.5f - yr) * .3f;
	} else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
		yr += (-1.5f - yr) * .3f;
	} else {
		yr *= .9f;
	}
	rotate(camera, {0, yr, 0});

	static float xr;
	if (window->keyboard()->key(KeyCode::UP).down()) {
		xr += (-1.5f - xr) * .3f;
	} else if (window->keyboard()->key(KeyCode::DOWN).down()) {
		xr += (1.5f - xr) * .3f;
	} else {
		xr *= .9f;
	}
	turn(camera, {xr, 0, 0});

	static float vz;
	if (window->keyboard()->key(KeyCode::W).down()) {
		vz += (.5f - vz) * .3f;
	} else if (window->keyboard()->key(KeyCode::S).down()) {
		vz += (-.5f - vz) * .3f;
	} else {
		vz *= .9f;
	}
	move(camera, {0, 0, vz});

	static float vx;
	if (window->keyboard()->key(KeyCode::A).down()) {
		vx += (-.5f - vz) * .3f;
	} else if (window->keyboard()->key(KeyCode::D).down()) {
		vx += (.5f - vz) * .3f;
	} else {
		vx *= .9f;
	}
	move(camera, {vx, 0, 0});
}

void start(void (*entry)()) {

	window = new Window({1024, 768}, "Hello world!", sgd::WindowFlags::resizable | sgd::WindowFlags::centered);

	//	window = new Window({768,768}, "Hello world!", sgd::WindowFlags::none);//resizable);

	SGD_LOG << "Window size" << window->size();

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [](CVec2u) {
		if (scene) render();
	});

	gc = new GraphicsContext(window, wgpu::BackendType::D3D12);

	scene = new Scene(gc);
	scene->ambientLightColor = {1, .9, .8, .1};

	auto skyTexture = loadTexture(Path("sgd://envmaps/sunnysky-cube.png"), TextureFormat::srgba8,
								  TextureFlags::cube | TextureFlags::mipmap | TextureFlags::filter)
						  .result();

	scene->envTexture = skyTexture;

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = skyTexture;
	scene->add(skybox);

	LightPtr light = new Light(LightType::directional);
	turn(light, {-45, 225, 0});
	scene->add(light);

	entry();
}
