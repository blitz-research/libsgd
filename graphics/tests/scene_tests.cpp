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

	//	window->sizeChanged.connect(nullptr, [](CVec2u) {
	//		if (scene) scene->render();
	//	});

	gc = new GraphicsContext(window);

	scene = new Scene(gc);
	scene->clearColor = {1, 1, 0, 1};
	scene->ambientLightColor = {1, 1, 1, 0};

	scene->envTexture =
		loadTexture(Path("sunnysky-cube.png"), TextureFormat::srgba8, TextureFlags::cube | TextureFlags::mipmap).result();

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = scene->envTexture;
	skybox->roughness = .5f;
	scene->add(skybox);

	MeshPtr mesh = loadGLTFMesh(Path("helmet.glb")).result();
	fit(mesh, {-1, 1}, true);

	ModelPtr model = new Model();
	move(model, {0, 0, 3});
	model->mesh = mesh;
	scene->add(model);

	LightPtr light = new Light();
	light->setWorldMatrix(AffineMat4f::TRS({0, 10, 0}));
	light->color = {1, 1, 1, 1};
	light->range = 50;
	scene->add(light);

	while (window->pollEvents()) {
		turn(model, {0, .001, 0});
		render();
	}
}

int main() {

	std::thread(start).detach();

	beginAppEventLoop();
}
