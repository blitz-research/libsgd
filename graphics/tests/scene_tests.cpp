#include <graphics/exports.h>

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

	window->sizeChanged.connect(nullptr, [](CVec2u) {
		if (scene) {
			scene->render();

			gc->present(gc->colorBuffer());
		}
	});

	window->closeClicked.connect(nullptr, []{std::exit(0);});

	gc = new GraphicsContext(window);

	scene = new Scene(gc);
	scene->clearColor = {1,1,0,1};
	scene->ambientLightColor = {1, 1, 1, 0};

	scene->envTexture =
		loadTexture(Path("sunnysky-cube.png"), TextureFormat::srgba8, TextureFlags::cube | TextureFlags::filter).result();

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture  = scene->envTexture;
	scene->add(skybox);

	CameraPtr camera = new Camera();
	//scene->add(camera);

	//MaterialPtr material = loadMaterial(Path("Bricks076C_1K-JPG")).result();
	MaterialPtr material = loadMaterial(Path("Marble008_1K-JPG")).result();

	//	MeshPtr mesh = createSphereMesh(1, 64, 32, material);
//	MeshPtr mesh = createBoxMesh(Boxf({-20, -2, -20}, {20, -1, 20}), material);

	float sz=1.9f;
	MeshPtr mesh = createBoxMesh(Boxf({-sz, -sz, -sz}, {sz, sz, sz}), material);
	transformTexCoords(mesh, {4, 4}, {0, 0});

	sz = 25;
	for(float z=-sz;z<=sz;z+=2) {
		for(float x=-sz;x<=sz;x+=2) {
			ModelPtr model = new Model();
			model->setWorldMatrix(AffineMat4f::TRS({x, -2, z}));
			model->mesh = mesh;
			scene->add(model);
		}
	}

	LightPtr light = new Light();
	light->setWorldMatrix(AffineMat4f::TRS({0, 10, 0}));
	light->color = {1, 1, 1, 1};
	light->range = 50;
	scene->add(light);

	while(window->pollEvents()) {
//		turn( model, {0,.001,0});
		render();
	}
}

int main() {

	wgpu::RequestAdapterOptions opts{};
	opts.backendType = wgpu::BackendType::Vulkan;

	requestWGPUDevice(opts, [](const wgpu::Device& device) {

		start();
	});

	beginAppEventLoop();
}
