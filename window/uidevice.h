#pragma once

#include <geom/exports.h>

struct GLFWwindow;

namespace sgd {

SGD_SHARED(Window);

class Button : public Object {
public:
	Signal<float> valueChanged;
	Signal<> pressed;

	float value() const {
		return m_value;
	}

	bool down() const {
		return m_down;
	}

	bool hit() const {
		return m_hit;
	}

private:
	friend class UIDevice;

	float m_value = 0;
	bool m_down = false;
	bool m_hit = false;
};

class UIDevice : public Object {
public:
	static constexpr float buttonDownThreshold = .1f;

	uint32_t maxButtons() const {
		return m_maxButtons;
	}

	Button& button(uint32_t index) const {
		SGD_ASSERT(index < m_maxButtons);
		return m_buttons[index];
	}

	void flush();

	void resetButtonHits();

	void update();

protected:
	UIDevice(uint32_t maxButtons);
	virtual ~UIDevice() = default;

	void setButtonValue(uint32_t index, float value);

	void setButtonDown(uint32_t index, bool down);

	virtual void onPoll() {
	}

private:
	friend class Window;

	uint32_t m_maxButtons;
	Button* m_buttons;
	Vector<uint32_t> m_hits;

	void setButtonState(uint32_t index, float value, bool down);
};

} // namespace sgf
