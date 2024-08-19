#include "skyboxbindings.h"

#include "skyboxbindgroup.h"
#include "../core/textureutil.h"
#include "../material/nullmaterial.h"
#include "../render/renderqueue.h"

namespace sgd {

SkyboxBindings::SkyboxBindings() {

	// Clear material
	m_material = new Material(&nullMaterialDescriptor);
	m_material->blendMode = BlendMode::opaque;
	m_material->depthFunc = DepthFunc::always; // undefined;
	m_material->cullMode = CullMode::none;

	m_bindGroup = new BindGroup(&skyboxBindGroupDescriptor);

	m_uniforms = new Buffer(BufferType::uniform, nullptr, sizeof(SkyboxUniforms));
	m_bindGroup->setBuffer(0, m_uniforms);

	lockUniforms() = {};
	unlockUniforms();

	skyTexture.changed.connect(this, [=](CTexture* ctexture) { //
		if(!ctexture) ctexture = blackTexture(TextureFlags::cube);
		int type;
		if(bool(ctexture->flags() & TextureFlags::cube)) {
			m_bindGroup->setTexture(1, ctexture, false);
			m_bindGroup->setTexture(2, blackTexture(), false);
			m_bindGroup->setSampler(3, ctexture);
			type = 1;
		}else {
			m_bindGroup->setTexture(1, blackTexture(TextureFlags::cube), false);
			m_bindGroup->setTexture(2, ctexture);
			type = 2;
		}
		if(type == uniforms().textureType) return;
		lockUniforms().textureType = type;
		unlockUniforms();
	});
	skyTexture.changed.emit(skyTexture());

	worldMatrix.changed.connect(nullptr, [=](CAffineMat4f m){
		lockUniforms().worldMatrix = m;
		unlockUniforms();
	});

	roughness.changed.connect(nullptr,[=](float r){
		lockUniforms().mipmapBias = r * 15.999f;
		unlockUniforms();
	});
}

SkyboxUniforms& SkyboxBindings::lockUniforms() {
	return *(SkyboxUniforms*)m_uniforms->lock(0,sizeof(SkyboxUniforms));
}

void SkyboxBindings::unlockUniforms() {
	m_uniforms->unlock();
}

void SkyboxBindings::render(RenderQueue* rq) const {
	rq->addRenderOp(nullptr, nullptr, m_material, m_bindGroup, emptyBindGroup(BindGroupType::renderer), 4, 1, 0, 0, false);
}

}
