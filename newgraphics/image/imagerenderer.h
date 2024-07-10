#pragma once

#include "image.h"

namespace sgd {

struct alignas(16) ImageInstance {
	Mat4f matrix;
	Vec4f color;
	float frame{};
};

SGD_SHARED(RenderContext);

SGD_SHARED(ImageRenderer);

struct ImageRenderer : Shared {
	SGD_OBJECT_TYPE(ImageRenderer, Shared);

#if 0
	void clearInstances();

	ImageInstance* lockInstances(Image* image, uint32_t count);

	void unlockInstances();

	void render(RenderContext* rc) const;
#endif

private:
	struct InstOp {
		Image* image;
		uint32_t count;
	};

	InstOp m_instOps;

	BufferPtr m_instanceBuffer;
	uint32_t m_instanceCapacity;
};

}
