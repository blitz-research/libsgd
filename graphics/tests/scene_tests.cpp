#include <graphics/exports.h>

using namespace sgd;

WindowPtr window;
GraphicsContextPtr gc;
ScenePtr scene;

void render() {
	if (!window->pollEvents()) std::exit(0);

	requestRender(render);

	scene->render();

	gc->present(gc->colorBuffer());
}

void start() {

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::resizable);

	window->sizeChanged.connect(nullptr,[](CVec2u){
		if(scene) {
			scene->render();

			gc->present(gc->colorBuffer());
		}
	});

	gc = new GraphicsContext(window);

	scene = new Scene(gc);
	scene->ambientLightColor = {1, 1, 1, 0};
	scene->envTexture = loadTexture(Path("sunnysky-cube.png"), TextureFormat::srgba8, TextureFlags::cube | TextureFlags::filter).result();

	CameraPtr camera = new Camera();
	camera->setWorldMatrix(AffineMat4f::TRS({0,0,-3}));
	scene->add(camera);

	LightPtr light0 = new Light();
	light0->setWorldMatrix(AffineMat4f::TRS({2.5, 2.5, -2.5}));
	light0->color = {1, 1, 1, 1};
	light0->range = 25;
	scene->add(light0);

	LightPtr light1 = new Light();
	light1->setWorldMatrix(AffineMat4f::TRS({2.5, 2.5, -2.5}));
	light1->color = {1, 1, 1, 1};
	light1->range = 25;
	scene->add(light1);

	LightPtr light2 = new Light();
	light2->setWorldMatrix(AffineMat4f::TRS({2.5, 2.5, -2.5}));
	light2->color = {1, 1, 1, 1};
	light2->range = 25;
	scene->add(light2);

	//MaterialPtr material = loadMaterial(Path("Bricks076C_1K-JPG"));
	MaterialPtr material = loadMaterial(Path("Marble008_1K-JPG")).result();
	// MaterialPtr material = loadMaterial(Path("Facade001_1K-JPG"));
	//MaterialPtr material = loadMaterial(Path("Facade018A_1K-JPG"));
	//MaterialPtr material = loadMaterial(Path("PavingStones131_1K-JPG"));

	MeshPtr mesh = createSphereMesh(1, 64, 32, material);
	//MeshPtr mesh = createBoxMesh(Boxf(-1, 1), material);

	ModelPtr model = new Model();
	model->mesh = mesh;
	scene->add(model);

	requestRender(render);
}

int main() {

	wgpu::RequestAdapterOptions opts{};
	opts.backendType = wgpu::BackendType::Vulkan;

	requestWGPUDevice(opts, [](const wgpu::Device&) { start(); });

	beginAppEventLoop();
}
