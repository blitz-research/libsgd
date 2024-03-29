#pragma once

#include <geom/exports.h>

struct window;

namespace sgd {

SGD_SHARED(Window);

class Button : public Object {
public:
	Signal<> pressed;

	bool down() const {
		return m_down;
	}

	bool hit() const {
		return m_hit;
	}

private:
	friend class UIDevice;

	bool m_down = false;
	bool m_hit = false;
};

class UIDevice : public Object {
public:
	uint32_t maxButtons() const {
		return m_maxButtons;
	}

	Button& button(uint32_t index) const {
		SGD_ASSERT(index < m_maxButtons);
		return m_buttons[index];
	}

	void flush();

protected:
	explicit UIDevice(uint32_t maxButtons);
	~UIDevice() override = default;

	void setButtonDown(uint32_t index, bool down);

	void resetKeyHits();

private:
	uint32_t m_maxButtons;
	Button* m_buttons;
	Vector<uint32_t> m_hits;
};

} // namespace sgd
