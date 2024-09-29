#include "start.cpp"

float normalizeAngle(float d) {
	d = (float)std::fmod(d, 360);
	if (d >= 180) return d - 360;
	if (d < -180) return d + 360;
	return d;
}

float dir(float src, float dst) {
	return normalizeAngle(normalizeAngle(dst) - normalizeAngle(src));
}

float clampPitch(float ang, float min, float max) {
	float dmin = dir(ang, min);
	if (dmin > 0) return dmin;
	float dmax = dir(ang, max);
	if (dmax < 0) return dmax;

	return 0;
}

float clampYaw(float ang, float min, float max) {

	if (0){//dir(min, max) > 0) { // std::abs(max - min) >= 180) {
		float dmin = dir(ang, min);
		float dmax = dir(ang, max);
		if (dmin >= 0 || dmax <= 0) return 0;
		return std::abs(dmin) < std::abs(dmax) ? dmin : dmax;
	} else {
		float dmin = dir(ang, min);
		if (dmin < 0) return dmin;
		float dmax = dir(ang, max);
		if (dmax > 0) return dmax;
		return 0;
	}
}

void entry() {

	setConfigVar("gltf.loggingEnabled", "1");

	float size = 100.0f;

	MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://misc/grass1K.jpg")).result();
	MeshPtr groundMesh = createBoxMesh({{-size, 0, -size}, {size, 0, size}}, groundMaterial);
	transformTexCoords(groundMesh, {size / 2, size / 2}, {0, 0});

	ModelPtr groundModel = new Model(groundMesh);

	scene->add(groundModel);

	ModelPtr cannonModel = loadBonedModel(Path("~/Desktop/cannon.glb")).result();
	scene->add(cannonModel);

	Entity* turret = cannonModel->findChild("Turret");

	createPlayer(nullptr);
	move(player, {0, 1, -20});

	ImagePtr image = loadImage(Path("~/Desktop/StillLife.exr")).result();
	image->rect = {-.1,-.1,.1,.1};

	auto dc = overlay->drawList();

//	BloomEffectPtr effect = new BloomEffect();
	//effect->radius = 31;
	//scene->sceneRenderer()->add(effect);

	for (;;) {
		pollEvents();

		auto pitch = rotation(turret).x;
		auto yaw = rotation(turret).y;

		aim(turret, player->worldPosition(), 0);

//		rotate(turret, {0,1,0});
//		setRotation(turret, {0,rotation(turret).y+1,0});

		//		auto dir = ::dir(yaw, -160);
		auto pitchDir = clampPitch(pitch, 10, 30);

		auto yawDir = clampYaw(yaw, -90-45, 90+45);
//		if(yawDir<-1.1f) yawDir=-1.1f;
//		if(yawDir>1.1f) yawDir=1.1f;

		//auto yawDir = clampYaw(yaw, -45, 45);

		setRotation(turret, {pitch+pitchDir, yaw+yawDir, 0});
//		rotate(turret, {-pitchDir, yawDir, 0});

		pitch = rotation(turret).x;
		yaw = rotation(turret).y;

		playerFly(.125f);

		dc->clear();
//		dc->addImage(image,window->mouse()->position().xy(),0);
		dc->addText("Pitch:" + toString(pitch) + " pitchDir:" + toString(pitchDir), {0, 0});
		dc->addText("Yaw:" + toString(yaw) + " yawDir:" + toString(yawDir), {0, 16});

		render();
	}
}

int main() {
	start(entry);
}
