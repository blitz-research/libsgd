#pragma once

#include "texture.h"

namespace sgd {

Expected<Texture*, FileioEx> loadTexture(CData data, TextureFormat format, TextureFlags flags, uint32_t depth = 0);

Expected<Texture*, FileioEx> loadTexture(CPath path, TextureFormat format, TextureFlags flags, uint32_t depth = 0);

CTexture* rgbaTexture(uint32_t rgba, TextureFlags flags = TextureFlags::none);

CTexture* dummyTexture(TextureFormat format, TextureFlags flags);

inline CTexture* blackTexture(TextureFlags flags = TextureFlags::none) {
	return rgbaTexture(0xff000000, flags);
}

inline CTexture* whiteTexture(TextureFlags flags = TextureFlags::none) {
	return rgbaTexture(0xffffffff, flags);
}

inline CTexture* flatNormalTexture(TextureFlags flags = TextureFlags::none) {
	return rgbaTexture(0xffff8080, flags);
}

void premultiplyAlpha(void* data, TextureFormat format, CVec2u size, uint32_t pitch);

} // namespace sgd
