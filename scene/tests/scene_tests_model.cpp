#include "start.cpp"

void entry() {

	// Height of eiffel tower!
	float size = 330.0f;

	{
		MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://materials/Gravel023_1K-JPG")).result();
		groundMaterial->setFloat("roughness", 1);

		PlaneEntityPtr groundPlane = new PlaneEntity();
		groundPlane->bindings()->material = groundMaterial;
		scale(groundPlane,{4,4,4});
		scene->add(groundPlane);

		ColliderPtr collider = new PlaneCollider(groundPlane, 0, Planef(Vec3f(0,1,0),0));
	}

	{
		MeshPtr towerMesh = loadStaticMesh(Path("sgd://models/eiffel_tower.glb")).result();
		fit(towerMesh, {{-size / 2, 0, -size / 2}, {size / 2, size, size / 2}}, true);
		towerMesh->shadowsEnabled = true;

		ModelPtr towerModel = new Model(towerMesh);
		scene->add(towerModel);

		ColliderPtr collider = new MeshCollider(towerModel, 0, getOrCreateMeshColliderData(towerMesh));
	}

	{
		createPlayer(nullptr);
		move(player, {0, 10, -100});

		ColliderPtr collider = new SphereCollider(player, 1, 1);
	}

	scene->collisionSpace()->enableCollisions(1, 0, CollisionResponse::slide);

	light->setWorldPosition({0, 0, 0});
	light->shadowsEnabled = true;
	setRotation(light, {-45, 0, 0});

	auto& config = scene->sceneRenderer()->sceneBindings()->lockConfigUniforms();
	config.csmTextureSize = 4096;
	config.csmClipRange = 660.0f;
	config.csmDepthBias = 0.0001f;
	scene->sceneRenderer()->sceneBindings()->unlockConfigUniforms();

	FogEffectPtr fog = new FogEffect();
	scene->sceneRenderer()->add(fog);

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(.125f);

		scene->collisionSpace()->updateColliders();

		dc->clear();
		dc->addText("FPS:" + std::to_string(currentGC()->FPS()), {0, 0});
		dc->addText("RPS:" + std::to_string(scene->sceneRenderer()->RPS()), {0, 16});

		rotate(light, {0, .025f, 0});

		render();
	}
}

int main() {
	start(entry);
}
