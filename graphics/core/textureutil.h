#pragma once

#include "texture.h"

namespace sgd {

Expected<TextureData*, FileioEx> loadTextureData(CData data, TextureFormat format);

Expected<TextureData*, FileioEx> loadTextureData(CPath path, TextureFormat format);

Expected<Texture*, FileioEx> load2DTexture(CPath path, TextureFormat format, TextureFlags flags);

Expected<Texture*, FileioEx> loadArrayTexture(CPath path, TextureFormat format, TextureFlags flags);

Expected<Texture*, FileioEx> loadArrayTexture(CPath path, TextureFormat format, TextureFlags flags, uint32_t frameCount,
											  uint32_t framesX, uint32_t framesY, uint32_t frameSpacing);

Expected<Texture*, FileioEx> loadCubeTexture(CPath path, TextureFormat format, TextureFlags flags);

CTexture* dummyTexture(TextureFormat format, TextureType type = TextureType::e2d);

CTexture* rgbaTexture(uint32_t rgba, TextureType type = TextureType::e2d);

CTexture* blackTexture(TextureType type = TextureType::e2d);

CTexture* whiteTexture(TextureType type = TextureType::e2d);

CTexture* flatTexture(TextureType type = TextureType::e2d);

} // namespace sgd
