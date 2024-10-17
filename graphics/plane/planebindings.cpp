#include "planebindings.h"

#include "../material/prelitmaterial.h"
#include "../render/renderqueue.h"
#include "planebindgroup.h"

namespace sgd {

PlaneBindings::PlaneBindings()
	: m_bindGroup(new BindGroup(&planeBindGroupDescriptor)),
	  m_uniformBuffer(new Buffer(BufferType::uniform, nullptr, sizeof(PlaneUniforms))) {

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	worldMatrix.changed.connect(nullptr, [=](CAffineMat4f m) {
		auto& uniforms = lockUniforms();
		uniforms.worldMatrix = m;
		uniforms.invWorldMatrix = inverse(m);
		uniforms.worldNormal = Vec4f(m.r.j, 0);
		unlockUniforms();
	});
	worldMatrix.update();

	color.changed.connect(nullptr, [=](CVec4f c) {
		lockUniforms().color = c;
		unlockUniforms();
	});
	color.update();

	m_defaultMaterial = new Material(&prelitMaterialDescriptor);
}

const PlaneUniforms& PlaneBindings::uniforms() const {
	return *(const PlaneUniforms*)m_uniformBuffer->data();
}

PlaneUniforms& PlaneBindings::lockUniforms() const {
	return *(PlaneUniforms*)m_uniformBuffer->lock();
}

void PlaneBindings::unlockUniforms() const {
	m_uniformBuffer->unlock();
}

void PlaneBindings::render(RenderQueue* rq) const {

	auto m = material();

	rq->addRenderOp(nullptr, nullptr, m ? m : m_defaultMaterial, m_bindGroup, emptyBindGroup(BindGroupType::renderer), 4, 1, 0,
					0, false);
}

} // namespace sgd
