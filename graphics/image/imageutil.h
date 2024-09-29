#pragma once

#include "image.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path);

Expected<Image*, FileioEx> loadArrayImage(CPath path, uint32_t frameCount, uint32_t framesX, uint32_t framesY, uint32_t frameSpacing);

}
