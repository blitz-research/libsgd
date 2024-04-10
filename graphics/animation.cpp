#include "animation.h"

namespace sgd {

namespace {

Vec3f blend(const Vec3fAnimationKey& pkey, const Vec3fAnimationKey& key, float time) {
	float alpha = (time - pkey.time) / (key.time - pkey.time);
	return pkey.value + (key.value - pkey.value) * alpha;
}

Quatf blend(const QuatfAnimationKey& pkey, const QuatfAnimationKey& key, float time) {
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

} // namespace

Vec3f AnimationSeq::evaluatePosition(float time, CVec3f def) const {
	if(positionKeys.empty()) return def;
	return evaluate(positionKeys, time);
}

Quatf AnimationSeq::evaluateRotation(float time, CQuatf def) const {
	if(rotationKeys.empty()) return def;
	return evaluate(rotationKeys, time);
}

Vec3f AnimationSeq::evaluateScale(float time, CVec3f def) const {
	if(scaleKeys.empty()) return def;
	return evaluate(scaleKeys, time);
}

} // namespace sgd
