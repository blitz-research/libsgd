#include "start.cpp"

void entry() {

	CameraPtr camera= new Camera();
 	scene->add(camera);

	TexturePtr texture =
		loadTexture(Path("sgd://misc/grass1.png"), TextureFormat::srgba8, TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clampU | TextureFlags::clampV)
			.result();

	MaterialPtr material = new Material(&prelitMaterialDescriptor);
	material->setTexture("albedoTexture", texture);
	material->blendMode = BlendMode::alpha;

	for (int i = 0; i < 60000; ++i) {

		SpritePtr sprite = new Sprite();
		sprite->material = material;
		sprite->rect = Rectf(-.5, 0, .5, 1);
		scene->add(sprite);

		turn(sprite, {0, rnd(360), 0});
		move(sprite, {0, -1.5, rnd(100)});
	}

	for(;;){
		pollEvents();
		turn(camera,{0,1,0});
		render();
	}
}

int main() {
	start(entry);
}
