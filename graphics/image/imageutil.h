#pragma once

#include "image.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, uint32_t depth);

}
