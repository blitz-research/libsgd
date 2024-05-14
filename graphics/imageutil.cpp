#include "imageutil.h"

#include "textureutil.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, TextureFormat format, TextureFlags flags, uint32_t frames) {
	auto r = loadTexture(path, format, TextureFlags::array | flags, frames);
	if (!r) return r.error();

	auto texture = r.result();

	auto image = new Image();
	image->frames = texture;

	return image;
}

} // namespace sgd
