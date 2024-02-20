#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Scene);

SGD_SHARED(Entity);

struct Entity : Shared {
	SGD_OBJECT_TYPE(Entity, Shared);

	Scene* scene() const {
		return m_scene;
	}

	void setWorldMatrix(CAffineMat4f matrix);

	CAffineMat4f& worldMatrix() const {
		return m_worldMatrix;
	}

	void setLocalMatrix(CAffineMat4f matrix);

	CAffineMat4f localMatrix() const {
		return m_localMatrix;
	}

	void setEnabled(bool enabled);

	bool enabled() const {
		return m_enabled;
	}

	void setVisible(bool visible);

	bool visible() const {
		return m_visible;
	}

protected:
	virtual void onCreate(){};
	virtual void onEnable(){};
	virtual void onShow(){};
	virtual void onHide(){};
	virtual void onDisable(){};
	virtual void onDestroy(){};

private:
	friend class Scene;

	AffineMat4f m_worldMatrix;
	AffineMat4f m_localMatrix;

	ScenePtr m_scene;

	bool m_enabled{};
	bool m_visible{};
};

} // namespace sgd
