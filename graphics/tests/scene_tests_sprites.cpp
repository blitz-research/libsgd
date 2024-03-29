#include "start.cpp"

void entry() {

	TexturePtr texture =
		loadTexture(Path("~/dev/assets/grass.png"), TextureFormat::srgba8, TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clampU | TextureFlags::clampV)
			.result();

	MaterialPtr material = new Material(&pbrMaterialDescriptor);
	material->setTexture("albedoTexture", texture);
	material->blendMode = BlendMode::alpha;

	for (int i = 0; i < 10000; ++i) {

		SpritePtr sprite = new Sprite();
		sprite->material = material;
		sprite->rect = Rectf(-.5, 0, .5, 1);
		scene->add(sprite);

		turn(sprite, {0, rnd(360), 0});
		move(sprite, {0, -1.5, rnd(100)});
	}

	//	move(sprite, {0, 0, 3});

	while (window->pollEvents()) {
		render();
	}
}

int main() {
	start(entry);
}
