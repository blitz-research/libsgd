#pragma once

#include "device.h"

namespace sgd {

void generateMipmaps(const wgpu::Device& device, const wgpu::Texture& texture);

}
