#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Scene);

SGD_SHARED(Entity);

struct Entity : Shared {
	SGD_OBJECT_TYPE(Entity, Shared);

	Entity() = default;

	void setWorldMatrix(CAffineMat4f matrix);

	CAffineMat4f& worldMatrix() const {
		if (bool(m_dirty & Dirty::worldMatrix)) validateWorldMatrix();
		return m_worldMatrix;
	}

	void setLocalMatrix(CAffineMat4f matrix);

	CAffineMat4f localMatrix() const {
		if (bool(m_dirty & Dirty::localMatrix)) validateLocalMatrix();
		return m_localMatrix;
	}

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	CVector<EntityPtr> children() const {
		return m_children;
	}

	void setEnabled(bool enabled);

	bool enabled() const {
		return m_enabled;
	}

	void setVisible(bool visible);

	bool visible() const {
		return m_visible;
	}

	Scene* scene() const {
		return m_scene;
	}

	Entity* copy() const {
		return onCopy();
	}

protected:
	explicit Entity(const Entity* that);

	virtual void onCreate(){};
	virtual void onEnable(){};
	virtual void onShow(){};
	virtual void onHide(){};
	virtual void onDisable(){};
	virtual void onDestroy(){};

	virtual Entity* onCopy() const = 0;

private:
	friend struct Scene;

	enum struct Dirty { none = 0, worldMatrix = 1, localMatrix = 2 };

	mutable AffineMat4f m_worldMatrix;
	mutable AffineMat4f m_localMatrix;
	mutable Dirty m_dirty{Dirty::none};

	Entity* m_parent{};
	Vector<EntityPtr> m_children;

	bool m_enabled{true};
	bool m_visible{true};

	Scene* m_scene{};

	void invalidateWorldMatrix() const;
	void invalidateLocalMatrix() const;
	void validateWorldMatrix() const;
	void validateLocalMatrix() const;
};

} // namespace sgd
