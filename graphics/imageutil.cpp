#include "imageutil.h"

#include "textureutil.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, uint32_t frames) {
	auto format  = TextureFormat::srgba8;
	auto flags = TextureFlags::array | TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto r = loadTexture(path, format, flags, frames);
	if (!r) return r.error();

	auto texture = r.result();

	auto image = new Image();
	image->frames = texture;

	return image;
}

} // namespace sgd
