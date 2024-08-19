#pragma once

#include "../core/bindgroup.h"

namespace sgd {

struct alignas(16) SkyboxUniforms {
	Mat4f worldMatrix;
	float mipmapBias{0};
	int textureType{2};	// 1 = cube, 2 = 2D

};

extern const BindGroupDescriptor skyboxBindGroupDescriptor;

}
