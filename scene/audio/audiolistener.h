#pragma once

#include "../scene/entity.h"

namespace sgd {

struct AudioListener : public EntityListener {
	SGD_OBJECT_TYPE(AudioListener, EntityListener);

	explicit AudioListener(Entity* entity);

private:
	friend struct AudioSpace;

	Vec3r m_srcpos;

	void onCreate() override;
	void onDestroy() override;
	void onReset() override;
	void update();
};

} // namespace sgd
