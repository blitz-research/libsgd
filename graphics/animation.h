#pragma once

#include <geom/exports.h>

namespace sgd {

enum struct AnimationMode {
	oneshot,
	loop,
	pingpong
};

template <class T> struct AnimationKey {
	float time;
	T value;
};
template <class T> using CAnimationKey = AnimationKey<T>;

using Vec3fAnimationKey = AnimationKey<Vec3f>;
using QuatfAnimationKey = AnimationKey<Quatf>;

// Don't really like this name...
SGD_SHARED(AnimationSeq);

struct AnimationSeq : public Shared {
	SGD_OBJECT_TYPE(AnimationSeq, Shared);

	Vector<Vec3fAnimationKey> positionKeys;
	Vector<QuatfAnimationKey> rotationKeys;
	Vector<Vec3fAnimationKey> scaleKeys;

	int const bone;

	AnimationSeq(int bone):bone(bone){}

	Vec3f evaluatePosition(float time, CVec3f def) const;
	Quatf evaluateRotation(float time, CQuatf def) const;
	Vec3f evaluateScale(float time, CVec3f def) const;
};

SGD_SHARED(Animation);

struct Animation : public Shared {
	SGD_OBJECT_TYPE(Animation, Shared);

	String const name;
	Vector<CAnimationSeqPtr> const sequences;
	float const duration;

	Animation(CString name, Vector<CAnimationSeqPtr> seqs, float dur) : name(name), sequences(seqs), duration(dur) {
	}
};

} // namespace sgd
