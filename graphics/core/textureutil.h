#pragma once

#include "texture.h"

namespace sgd {

Expected<TextureData*, FileioEx> loadTextureData(CData data, TextureFormat format);

Expected<TextureData*, FileioEx> loadTextureData(CPath path, TextureFormat format);

Expected<Texture*, FileioEx> load2DTexture(CPath path, TextureFormat format, TextureFlags flags);

Expected<Texture*, FileioEx> loadCubeTexture(CPath path, TextureFormat format, TextureFlags flags);

Expected<Texture*, FileioEx> loadArrayTexture(CPath path, TextureFormat format, TextureFlags flags, uint32_t depth = 0);

CTexture* rgbaTexture(uint32_t rgba, TextureFlags flags = TextureFlags::none);

CTexture* dummyTexture(TextureFormat format, TextureFlags flags);

inline CTexture* blackTexture(TextureFlags flags = TextureFlags::none) {
	return rgbaTexture(0xff000000, flags);
}

inline CTexture* whiteTexture(TextureFlags flags = TextureFlags::none) {
	return rgbaTexture(0xffffffff, flags);
}

inline CTexture* flatTexture(TextureFlags flags = TextureFlags::none) {
	return rgbaTexture(0xffff8080, flags);
}

} // namespace sgd
