#include "imageutil.h"

#include "../core/textureutil.h"

#include "imagematerial.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path) {

//	auto flags = TextureFlags::array | TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = loadArrayTexture(path, TextureFormat::any, TextureFlags::image);
	if (!texture) return texture.error();

	return new Image(texture.result());
}

Expected<Image*, FileioEx> loadArrayImage(CPath path, uint32_t frameCount, uint32_t framesX, uint32_t framesY,
									   uint32_t frameSpacing) {

	if (!frameCount || !framesX || !framesY) SGD_ERROR("Invalid frame layout parameters");

	auto texture = loadArrayTexture(path, TextureFormat::any, TextureFlags::image, frameCount, framesX, framesY, frameSpacing);
	if(!texture) return texture.error();

	return new Image(texture.result());
}

} // namespace sgd
