#pragma once

#include <geom/exports.h>

namespace sgd {

enum struct AnimationMode { oneshot = 1, loop, pingpong };

template <class T> struct AnimationKey {
	float time;
	T value;
};
template <class T> using CAnimationKey = AnimationKey<T>;

using Vec3fAnimationKey = AnimationKey<Vec3f>;
using QuatfAnimationKey = AnimationKey<Quatf>;

template <class T> T evaluate(CVector<AnimationKey<T>> keys, float time);

// Don't really like this name...
SGD_SHARED(AnimationSeq);

// TODO: Make immutable
struct AnimationSeq : public Shared {
	SGD_OBJECT_TYPE(AnimationSeq, Shared);

	Vector<Vec3fAnimationKey> positionKeys;
	Vector<QuatfAnimationKey> rotationKeys;
	Vector<Vec3fAnimationKey> scaleKeys;

	int const bone;

	explicit AnimationSeq(int bone) : bone(bone) {
	}
};

SGD_SHARED(Animation);

struct Animation : public Shared {
	SGD_OBJECT_TYPE(Animation, Shared);

	String const name;
	Vector<CAnimationSeqPtr> const sequences;
	float const duration;

	Animation(String name, Vector<CAnimationSeqPtr> seqs, float dur) : name(std::move(name)), sequences(std::move(seqs)), duration(dur) {
	}
};

// ***** Inline *****

inline Vec3f blend(const Vec3fAnimationKey& pkey, const Vec3fAnimationKey& key, float time) {
	float alpha = (time - pkey.time) / (key.time - pkey.time);
	return blend(pkey.value, key.value, alpha);
}

inline Quatf blend(const QuatfAnimationKey& pkey, const QuatfAnimationKey& key, float time) {
	float alpha = (time - pkey.time) / (key.time - pkey.time);
	return slerp(pkey.value, key.value, alpha);
}

template <class T> T evaluate(CVector<AnimationKey<T>> keys, float time) {
	if (keys.empty()) return {};
	for (auto it = keys.begin(); it != keys.end(); ++it) {
		if (time > it->time) continue;
		if (it == keys.begin()) return it->value;
		return blend(*(it - 1), *it, time);
	}
	return keys.back().value;
}

} // namespace sgd
