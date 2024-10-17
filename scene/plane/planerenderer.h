#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(PlaneEntity);
SGD_SHARED(RenderQueue);

SGD_SHARED(PlaneRenderer);

struct PlaneRenderer : Shared {
	SGD_OBJECT_TYPE(PlaneRenderer, Shared);

	void add(CPlaneEntity* plane);
	void remove(CPlaneEntity* plane);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	Vector<CPlaneEntityPtr> m_planes;
};

}
