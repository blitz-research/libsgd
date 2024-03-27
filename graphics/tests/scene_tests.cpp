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

	gc = new GraphicsContext(window, wgpu::BackendType::D3D12);

	scene = new Scene(gc);
	scene->ambientLightColor = {1, 1, 1, 0};

	auto skyTexture = loadTexture(Path("sgd://envmaps/sunnysky-cube.png"), TextureFormat::srgba8,
								  TextureFlags::cube | TextureFlags::mipmap | TextureFlags::filter)
						  .result();

	scene->envTexture = skyTexture;

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = skyTexture;
	scene->add(skybox);

	LightPtr light = new Light(LightType::directional);
	turn(light, {-halfPi / 2, 0, 0});
	scene->add(light);

#if 0
	CameraPtr camera = new Camera();
	camera->near = .1;
	camera->far = 1000;
	move(camera, {0, 1, -3});
	scene->add(camera);
#endif

	MaterialPtr material = loadPBRMaterial(Path("sgd://materials/Tiles019_1K-JPG")).result();
	MeshPtr groundMesh = createBoxMesh(Boxf{{-10, -3, -10}, {10, -2, 10}}, material);
	transformTexCoords(groundMesh, {3, 3}, {0, 0});
	ModelPtr ground = new Model();
	ground->mesh = groundMesh;
	scene->add(ground);

	auto mesh = loadStaticMesh(Path("sgd://models/helmet.glb")).result();
	fit(mesh, Boxf{{-1, -1, -1}, {1, 1, 1}}, true);
	ModelPtr model = new Model();
	model->mesh = mesh;
	move(model, {0, 0, 3});
	scene->add(model);

	float time = 0;

	while (window->pollEvents()) {

		if (window->keyboard()->key((uint32_t)KeyCode::LEFT).down()) {
			turn(model, {0, -.1, 0});
		} else if (window->keyboard()->key((uint32_t)KeyCode::RIGHT).down()) {
			turn(model, {0, .1, 0});
		}

		if (window->keyboard()->key((uint32_t)KeyCode::UP).down()) {
			turn(model, {-.1, 0, 0});
		} else if (window->keyboard()->key((uint32_t)KeyCode::DOWN).down()) {
			turn(model, {.1, 0, 0});
		}

		render();
	}
}

int main() {

	std::thread(start).detach();

	beginAppEventLoop();
}
