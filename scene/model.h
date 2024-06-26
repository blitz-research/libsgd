#pragma once

#include "animation.h"
#include "entity.h"

namespace sgd {

SGD_SHARED(Mesh);
SGD_SHARED(Model);

struct Joint {
	AffineMat4f inverseBindMatrix;
	int bone;
};

struct Model : Entity {
	SGD_OBJECT_TYPE(Model, Entity);

	Model(Mesh* mesh=nullptr);

	explicit Model(const Model* that);;

	Model(Vector<EntityPtr> bones, Vector<AnimationPtr> animations, Vector<Joint> joints);

	Property<MeshPtr> mesh;

	Property<Vec4f> color{Vec4f{1, 1, 1, 1}};

	void animate(uint32_t index, float time, AnimationMode mode, float weight);

	CVector<AffineMat4r> jointMatrices() const {return m_jointMatrices;}

private:
	Vector<AnimationPtr> m_animations;
	Vector<EntityPtr> m_bones;
	Vector<Joint> m_joints;
	bool m_skinned{};
	Vector<AffineMat4r> m_jointMatrices;

	void onCreate() override;
	void onShow() override;
	void onHide() override;

	Model* onCopy() const override;
};

} // namespace sgd
