#pragma once

#include "device.h"

namespace sgd {

void drawTexture(const wgpu::Device& device, const wgpu::Texture& srcTexture, CRectu srcRect, const wgpu::Texture& dstTexture, CRectu dstRect);

void copyTexture(const wgpu::Device& device, const wgpu::Texture& srcTexture, CVec2u srcOrigin, const wgpu::Texture& dstTexture,
				 CVec2u dstOrigin, CVec2u size);

void copyTexture(const wgpu::Device& device, const wgpu::Texture& srcTexture, const wgpu::Texture& dstTexture);

} // namespace sgd
