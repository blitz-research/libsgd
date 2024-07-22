#include "model.h"

#include "modelrenderer.h"
#include "skinnedmodelrenderer.h"

#include "../scene/scenerenderer.h"

namespace sgd {

Model::Model(Mesh* mmesh) : mesh(mmesh) {
}

Model::Model(CModel* that)
	: Entity(that),					   //													//
	  mesh(that->mesh()),			   //
	  color(that->color()),			   //
	  m_bones(that->m_bones),		   //
	  m_animations(that->m_animations) // We need an AnimationSet
{
	if (m_bones.empty()) return;

	// This is currently a bit crusty, we need to remap bone entities to their copies, assumes they've been copied by Entity
	// ctor 'in order'. Would like a nicer way to do this ultimately.
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
	for (auto& bone : m_bones) bone = bonesMap[bone];
}

Model::Model(Vector<EntityPtr> bones, Vector<AnimationPtr> animations, Vector<Joint> joints) //
	: m_bones(std::move(bones)),															 //
	  m_animations(std::move(animations)),													 //
	  m_joints(std::move(joints)),															 //
	  m_skinned(!m_joints.empty()),															 //
	  m_jointMatrices(m_joints.size()) {													 //
}

Model* Model::onCopy() const {
	return new Model(this);
}

void Model::onShow() {
	if (m_skinned) {
		scene()->sceneRenderer()->skinnedModelRenderer()->add(this);
	} else {
		SGD_LOG << "Show";
		scene()->sceneRenderer()->modelRenderer()->add(this);
	}
}

void Model::onHide() {
	if (m_skinned) {
		scene()->sceneRenderer()->skinnedModelRenderer()->remove(this);
	} else {
		SGD_LOG << "Hide";
		scene()->sceneRenderer()->modelRenderer()->remove(this);
	}
}

void Model::animate(uint32_t index, float time, AnimationMode mode, float weight) {

	SGD_ASSERT(index < m_animations.size());
	CAnimation* anim = m_animations[index];

	if (weight <= 0.0f) return;

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

	if (weight >= 1.0f) {
		for (CAnimationSeq* seq : anim->sequences) {
			auto bone = m_bones[seq->bone];
			if (!seq->positionKeys.empty()) bone->setLocalPosition(evaluate(seq->positionKeys, time));
			if (!seq->rotationKeys.empty()) bone->setLocalBasis(Mat3f::rotation(evaluate(seq->rotationKeys, time)));
			if (!seq->scaleKeys.empty()) bone->setLocalScale(evaluate(seq->scaleKeys, time));
		}
	} else {
		for (CAnimationSeq* seq : anim->sequences) {
			auto bone = m_bones[seq->bone];

			if (!seq->positionKeys.empty()) {
				auto pos = Vec3f(bone->localPosition());
				pos = sgd::blend(pos, evaluate(seq->positionKeys, time), weight);
				bone->setLocalPosition(pos);
			}
			if (!seq->rotationKeys.empty()) {
				auto rot = Quatf::rotation(bone->localBasis());
				rot = sgd::slerp(rot, evaluate(seq->rotationKeys, time), weight);
				bone->setLocalBasis(Mat3f::rotation(rot));
			}
			if (!seq->scaleKeys.empty()) {
				auto scl = Vec3f(bone->localScale());
				scl = sgd::blend(scl, evaluate(seq->scaleKeys, time), weight);
				bone->setLocalScale(scl);
			}
		}
	}

	for (int i = 0; i < m_joints.size(); ++i) {
		m_jointMatrices[i] = m_bones[m_joints[i].bone]->worldMatrix() * AffineMat4r(m_joints[i].inverseBindMatrix);
	}
}

} // namespace sgd
