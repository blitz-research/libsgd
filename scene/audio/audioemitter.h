#pragma once

#include "../scene/entity.h"

namespace sgd {

SGD_SHARED(Sound);
SGD_SHARED(Audio);

struct AudioEmitter : public EntityListener {
	SGD_OBJECT_TYPE(AudioEmitter, EntityListener);

	static AudioEmitter* getOrCreate(Entity* entity);

	uint32_t play3DSound(Sound* sound);


private:
	friend struct AudioSpace;

	Vec3r m_srcpos;
	Vector<uint32_t> m_audios{};

	void onCreate() override;
	void onDestroy() override;
	void onReset() override;
	void update();

	explicit AudioEmitter(Entity* entity);
};

} // namespace sgd
