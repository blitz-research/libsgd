#pragma once

#include "image.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, TextureFormat format, TextureFlags flags, uint32_t frames);

}
