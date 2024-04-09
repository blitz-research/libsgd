#include <graphics/exports.h>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;
ScenePtr scene;

int main() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	gc = new GraphicsContext(window, wgpu::BackendType::D3D12);

	scene = new Scene(gc);
	scene->ambientLightColor = {1, 1, 1, 0};

	auto skyTexture = loadTexture(Path("sgd://envmaps/sunnysky-cube.png"), TextureFormat::srgba8,
								  TextureFlags::envCube).result();
	scene->envTexture = skyTexture;

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = skyTexture;
	scene->add(skybox);

	MeshPtr mesh = loadStaticMesh(Path("sgd://models/helmet.glb")).result();

	ModelPtr model = new Model();
	model->mesh = mesh;
	move(model, {0, 0, 5});
	scene->add(model);

	LightPtr light = new Light(LightType::directional);
	turn(light, {-halfPi / 2, 0, 0});
	scene->add(light);

	for (;;) {

		if(window->keyboard()->key((uint32_t)KeyCode::SPACE).hit()) {
			ModelPtr model = new Model();
			model->mesh = mesh;
			move(model, {rnd(-5,5), 0, 5});
			scene->add(model);
		}

		pollEvents();

		scene->render();

		gc->present(gc->colorBuffer());
	}
}
