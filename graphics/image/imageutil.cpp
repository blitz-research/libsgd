#include "imageutil.h"

#include "../core/textureutil.h"

#include "imagematerial.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, uint32_t depth) {

	auto format = TextureFormat::srgba8;
	auto flags = TextureFlags::array | TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = loadTexture(path, format, flags, depth);
	if (!texture) return texture.error();

	auto material = new Material(&imageMaterialDescriptor);
	material->setTexture("albedoTexture", texture.result());
	material->blendMode = BlendMode::alphaBlend;
	material->cullMode = CullMode::none;

	auto image = new Image(material);

	return image;
}

} // namespace sgd
