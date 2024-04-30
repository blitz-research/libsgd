#include "model.h"

#include "modelrenderer.h"
#include "scene.h"
#include "skinnedmodelrenderer.h"

namespace sgd {

Model::Model(const Model* that)
	: Entity(that),					   //													//
	  mesh(that->mesh()),			   //
	  color(that->color()),			   //
	  m_bones(that->m_bones),		   //
	  m_animations(that->m_animations) // We need an AnimationSet
{
	SGD_PANIC("TODO");

	// This is currently a bit crusty, we need to remap bone entities to their copies, assumes they've been copied by Entity
	// ctor 'in order'. We need a nicer way to do this ultimately.
	//
	Map<Entity*, Entity*> bonesMap;
	Function<void(const Entity*, const Entity*)> mapBones;
	mapBones = [&](const Entity* e0, const Entity* e1) {
		SGD_ASSERT(e0->children().size() == e1->children().size()); // should be same after copy!
		for (int i = 0; i < e0->children().size(); ++i) {
			auto c0 = e0->children()[i];
			auto c1 = e1->children()[i];
			bonesMap[c0] = c1;
			mapBones(c0, c1);
		}
	};
	mapBones(this, that);
	for (int i = 0; i < m_bones.size(); ++i) m_bones[i] = bonesMap[m_bones[i]];
}

Model* Model::onCopy() const {
	return new Model(this);
}

Model::Model(Vector<EntityPtr> bones, Vector<AnimationPtr> animations, Vector<Joint> joints) //
	: m_bones(std::move(bones)),															 //
	  m_animations(std::move(animations)),													 //
	  m_joints(std::move(joints)),															 //
	  m_skinned(!m_joints.empty()),															 //
	  m_jointMatrices(m_joints.size()) {													 //
}

void Model::onCreate() {
	if (m_skinned) {
		if (!scene()->getRenderer(RendererType::skinnedModel))
			scene()->setRenderer(RendererType::skinnedModel, new SkinnedModelRenderer());
	} else {
		if (!scene()->getRenderer(RendererType::model)) scene()->setRenderer(RendererType::model, new ModelRenderer());
	}
}

void Model::onShow() {
	if (m_skinned) {
		scene()->getRenderer(RendererType::skinnedModel)->as<SkinnedModelRenderer>()->add(this);
	} else {
		scene()->getRenderer(RendererType::model)->as<ModelRenderer>()->add(this);
	}
}

void Model::onHide() {
	if (m_skinned) {
		scene()->getRenderer(RendererType::skinnedModel)->as<SkinnedModelRenderer>()->remove(this);
	} else {
		scene()->getRenderer(RendererType::model)->as<ModelRenderer>()->remove(this);
	}
}

void Model::animate(uint32_t index, float time, AnimationMode mode) {
	SGD_ASSERT(index < m_animations.size());
	auto anim = m_animations[index];

	switch (mode) {
	case AnimationMode::oneshot:
		time = std::min(std::max(time, 0.0f), anim->duration);
		break;
	case AnimationMode::loop:
		time = sgd::floorMod(time, anim->duration);
		break;
	case AnimationMode::pingpong:
		time = sgd::floorMod(time, anim->duration * 2);
		if (time >= anim->duration) time = anim->duration - (time - anim->duration);
		break;
	}

	for (auto seq : anim->sequences) {
		auto bone = m_bones[seq->bone];

		auto pos = seq->evaluatePosition(time, bone->localMatrix().t);
		auto rot = seq->evaluateRotation(time, Quatf::rotation(bone->localMatrix().r));
		auto scl = seq->evaluateScale(time, scale(bone->localMatrix().r));

		bone->setLocalPosition(pos);
		bone->setLocalBasis(Mat3f::rotation(rot));
		bone->setLocalScale(scl);
	}

	for (int i = 0; i < m_joints.size(); ++i) {
		m_jointMatrices[i] = m_bones[m_joints[i].bone]->worldMatrix() * AffineMat4r(m_joints[i].inverseBindMatrix);
	}
}

} // namespace sgd
