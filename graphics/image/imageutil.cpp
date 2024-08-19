#include "imageutil.h"

#include "../core/textureutil.h"

#include "imagematerial.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, uint32_t depth) {

	auto flags = TextureFlags::array | TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = loadArrayTexture(path, TextureFormat::any, flags, depth);
	if (!texture) return texture.error();

	return new Image(texture.result());
}

} // namespace sgd
