#pragma once

#include "texture.h"

namespace sgd {

inline uint32_t bytesPerTexel(TextureFormat format) {
	uint32_t bpp[]{0, 4, 4, 16, 4};
	return bpp[(int)format];
}

Texture* rgbaTexture(uint32_t rgba);

inline Texture* blackTexture() {
	return rgbaTexture(0xff000000);
}

inline Texture* whiteTexture() {
	return rgbaTexture(0xffffffff);
}

Expected<Texture*, FileioEx> loadTexture(CData data, TextureFormat format, TextureFlags flags);

Expected<Texture*, FileioEx> loadTexture(CPath path, TextureFormat format, TextureFlags flags);

void premultiplyAlpha(void* data, TextureFormat format, CVec2u size, uint32_t pitch);


} // namespace sgd
