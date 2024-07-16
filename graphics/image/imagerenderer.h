#pragma once

#include "image.h"

namespace sgd {

struct alignas(16) ImageInstance {
	Mat4f worldMatrix;
	Vec4f color;
	float frame{};
};

SGD_SHARED(RenderQueue);

SGD_SHARED(ImageRenderer);

struct ImageRenderer : Shared {
	SGD_OBJECT_TYPE(ImageRenderer, Shared);

	ImageRenderer();

	ImageInstance* lockInstances(uint32_t count);

	void unlockInstances();

	void render(RenderQueue* rq, CImage* image, uint32_t first, uint32_t count);

	BindGroup* bindGroup()const{
		return m_bindGroup;
	}

private:
	BindGroupPtr m_bindGroup;
	uint32_t m_instanceCapacity{};
	BufferPtr m_instanceBuffer;
};

}
