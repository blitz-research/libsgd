#pragma once

#include "texture.h"

namespace sgd {

inline uint32_t bytesPerTexel(TextureFormat format) {
	uint32_t bpp[]{0, 4, 4, 16, 4};
	return bpp[(int)format];
}

const Texture* rgbaTexture(uint32_t rgba);

inline const Texture* blackTexture() {
	return rgbaTexture(0xff000000);
}

inline const Texture* whiteTexture() {
	return rgbaTexture(0xffffffff);
}

inline const Texture* flatNormalTexture() {
	return rgbaTexture(0xffff8080);
}

Expected<Texture*, FileioEx> loadTexture(CData data, TextureFormat format, TextureFlags flags);

Expected<Texture*, FileioEx> loadTexture(CPath path, TextureFormat format, TextureFlags flags);

void premultiplyAlpha(void* data, TextureFormat format, CVec2u size, uint32_t pitch);

} // namespace sgd
