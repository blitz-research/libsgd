#pragma once

#include "image.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, uint32_t depth);

Expected<Image*, FileioEx> loadImageEx(CPath path, uint32_t frameCount, uint32_t frameCountX, uint32_t frameCountY, uint32_t frameSpacing);

}
