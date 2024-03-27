
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

	window = new Window({1280, 960}, "Hello world!", sgd::WindowFlags::none);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	gc = new GraphicsContext(window);

	scene = new Scene(gc);
	scene->clearColor = {1, 1, 0, 1};
	scene->ambientLightColor = {1, 1, 1, 0};

	scene->envTexture =
		loadTexture(Path("sgd://envmaps/sunnysky-cube.png"), TextureFormat::srgba8, TextureFlags::cube | TextureFlags::mipmap)
			.result();

	SkyboxPtr skybox = new Skybox();
	skybox->skyTexture = scene->envTexture;
	skybox->roughness = .4f;
	scene->add(skybox);

	LightPtr light = new Light();
	light->setWorldMatrix(AffineMat4f::TRS({0, 10, 0}));
	light->color = {1, 1, 1, 1};
	light->range = 50;
	scene->add(light);

	scene->add(new Camera());

	MaterialPtr redMaterial = createPBRMaterial();
	redMaterial->setVector4f("albedoColor4f", Vec4f(1, 0, 0, 1));

	MaterialPtr greenMaterial = createPBRMaterial();
	greenMaterial->setVector4f("albedoColor4f", Vec4f(0, 1, 0, 1));

	MaterialPtr blueMaterial = createPBRMaterial();
	blueMaterial->setVector4f("albedoColor4f", Vec4f(0, 0, 1, 1));

	ModelPtr root = new Model();
	root->setLocalMatrix(AffineMat4f::TRS({0, 0, 5}));
	root->mesh = createSphereMesh(.5, 64, 32, redMaterial);
	scene->add(root);

	ModelPtr child0 = new Model();
	child0->setParent(root);
	child0->setLocalMatrix(AffineMat4f::TRS({-1, -1, 0}));
	child0->mesh = createSphereMesh(.5, 64, 32, greenMaterial);
	scene->add(child0);

	ModelPtr child1 = new Model();
	child1->setParent(root);
	child1->setLocalMatrix(AffineMat4f::TRS({1, -1, 0}));
	child1->mesh = createSphereMesh(.5, 64, 32, blueMaterial);
	scene->add(child1);

	while (window->pollEvents()) {
		turn(root, {0, .001, 0});
		render();
	}
}

int main() {

	std::thread(start).detach();

	beginAppEventLoop();
}
