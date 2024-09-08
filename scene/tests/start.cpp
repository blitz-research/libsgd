#include <scene/exports.h>

#include <thread>

using namespace sgd;

WindowPtr window;
ScenePtr scene;
SkyboxPtr skybox;
LightPtr light;
OverlayPtr overlay;

ModelPtr player;
float player_vx;
float player_vy;
float player_vz;
float player_rvx;
float player_rvy;

CameraPtr camera;
float camera_rx;

void render() {
	auto gc = currentGC();

	scene->render();

	gc->present(scene->sceneRenderer()->outputTexture());
}

void start(void (*entry)()) {

	initApp();

	window = new Window({1024, 768}, "Hello world!", sgd::WindowFlags::resizable | sgd::WindowFlags::centered);
	//window = new Window(desktopSize(), "Hello world!", sgd::WindowFlags::fullscreen);

	window->closeClicked.connect(nullptr, [] { std::exit(0); });

	window->sizeChanged.connect(nullptr, [](CVec2u) {
		if (scene) render();
	});

	createGC(window);

	scene = new Scene();
	scene->sceneRenderer()->ambientLightColor = Vec4f(1, .9, .8, .1);

	//auto envPath = "~/Desktop/starmap_2020_4k.exr";
	//auto envTexture = loadTexture(Path(envPath), TextureFormat::any, TextureFlags::filter).result();

	//auto envPath = "sgd://envmaps/stormy-cube.jpg";
	auto envPath = "sgd://envmaps/sunnysky-cube.png";
	auto envTexture = loadCubeTexture(Path(envPath), TextureFormat::any, TextureFlags::mipmap|TextureFlags::filter).result();

	scene->sceneRenderer()->envTexture = envTexture;
	skybox = new Skybox(envTexture);
	scene->add(skybox);

	entry();
	return;

	light = new Light(LightType::directional);
	scene->add(light);
	turn(light, {-45, -45, 0});

	overlay = new Overlay();
	scene->add(overlay);

	entry();
}

void createPlayer(Mesh* mesh) {

	player = new Model();
	scene->add(player);
	player->mesh = mesh;

	camera = new Camera(CameraType::perspective);
	scene->add(camera);
	camera->setParent(player);
}

void playerWalk(float speed) {

	// Turn left/right
	if (window->keyboard()->key(KeyCode::LEFT).down()) {
		player_rvy += (1.5f - player_rvy) * .3f;
	} else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
		player_rvy += (-1.5f - player_rvy) * .3f;
	} else {
		player_rvy *= .9f;
	}
	rotate(player, {0, player_rvy, 0});

	// Look up/down
	if (window->keyboard()->key(KeyCode::UP).down()) {
		camera_rx += (45 - camera_rx) * .025f;
	} else if (window->keyboard()->key(KeyCode::DOWN).down()) {
		camera_rx += (-45 - camera_rx) * .025f;
	} else {
		camera_rx *= .9f;
	}
	setRotation(camera, {camera_rx, 0, 0});

	// Move forward/backward
	if (window->keyboard()->key(KeyCode::W).down()) {
		player_vz += (speed - player_vz) * .3f;
	} else if (window->keyboard()->key(KeyCode::S).down()) {
		player_vz += (-speed - player_vz) * .3f;
	} else {
		player_vz *= .9f;
	}
	move(player, {0, 0, player_vz});

	// Move left/right
	if (window->keyboard()->key(KeyCode::A).down()) {
		player_vx += (-speed - player_vx) * .3f;
	} else if (window->keyboard()->key(KeyCode::D).down()) {
		player_vx += (speed - player_vx) * .3f;
	} else {
		player_vx *= .9f;
	}
	move(player, {player_vx, 0, 0});
}

void playerFly(float speed) {

	// Turn left/right
	if (window->keyboard()->key(KeyCode::LEFT).down()) {
		player_rvy += (1.5f - player_rvy) * .3f;
	} else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
		player_rvy += (-1.5f - player_rvy) * .3f;
	} else {
		player_rvy *= .9f;
	}
	rotate(player, {0, player_rvy, 0});
	setRotation(camera, {0, 0, player_rvy * -15});

	// Turn up/down
	if (window->keyboard()->key(KeyCode::UP).down()) {
		player_rvx += (-1.5f - player_rvx) * .3f;
	} else if (window->keyboard()->key(KeyCode::DOWN).down()) {
		player_rvx += (1.5f - player_rvx) * .3f;
	} else {
		player_rvx *= .9f;
	}
	turn(player, {player_rvx, 0, 0});

	// Move forward/backward
	if (window->keyboard()->key(KeyCode::W).down()) {
		player_vz += (speed - player_vz) * .3f;
	} else if (window->keyboard()->key(KeyCode::S).down()) {
		player_vz += (-speed - player_vz) * .3f;
	} else {
		player_vz *= .9f;
	}
	move(player, {0, 0, player_vz});

	// Move left/right
	if (window->keyboard()->key(KeyCode::A).down()) {
		player_vx += (-speed - player_vx) * .3f;
	} else if (window->keyboard()->key(KeyCode::D).down()) {
		player_vx += (speed - player_vx) * .3f;
	} else {
		player_vx *= .9f;
	}
	move(player, {player_vx, 0, 0});
}
