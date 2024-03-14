#include <graphics/exports.h>

#include <thread>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;
ScenePtr scene;

void render() {

	scene->render();

	gc->present(gc->colorBuffer());
}

void start() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [](CVec2u) {
		if (scene) scene->render();
	});

	gc = new GraphicsContext(window, wgpu::BackendType::Vulkan);

	scene = new Scene(gc);
	scene->clearColor = {1, 1, 0, 1};
	scene->ambientLightColor = {1, 1, 1, 0};

	scene->envTexture =
		loadTexture(Path("sunnysky-cube.png"), TextureFormat::srgba8, TextureFlags::cube | TextureFlags::mipmap).result();

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = scene->envTexture;
	skybox->roughness = .5f;
	scene->add(skybox);

	LightPtr light = new Light();
	move(light, {0,0,-10});
	light->color = {1, 1, 1, 1};
	light->range = 50;
	scene->add(light);

	CameraPtr camera=new Camera();
	camera->near=.1;
	camera->far=1000;
	move(camera, {0,0,0});
	scene->add(camera);

	ModelPtr model = loadBonedModel(Path("boxanim.glb")).result();
//	ModelPtr model = loadStaticModel(Path("gearbox.glb")).result();

//	ModelPtr model = new Model();
//	auto mesh = loadGLTFMesh(Path("normaltangent.glb")).result();
//	fit(mesh, {-1,1}, true);
//	model->mesh=mesh;

	translate(model, {0,-1,3});
	scene->add(model);

	float time = 0;

	while (window->pollEvents()) {

		model->animate(0, time += 0.5f/60.0f, AnimationMode::loop);

		rotate(model, {0, .001, 0});

		render();
	}
}

int main() {

	std::thread(start).detach();

	beginAppEventLoop();
}
