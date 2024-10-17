#pragma once

#include "../core/bindgroup.h"

namespace sgd {

struct alignas(16) PlaneUniforms {
	Mat4f worldMatrix;
	Mat4f invWorldMatrix;
	Vec4f worldNormal;
	Vec4f color;
};

extern const BindGroupDescriptor planeBindGroupDescriptor;

}
