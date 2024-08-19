#pragma once

#include "../core/bindgroup.h"

namespace sgd {

struct alignas(16) ImageUniforms {
	Vec4f rect;
	uint32_t viewMode{};
};

const extern BindGroupDescriptor imageBindGroupDescriptor;

}
