#pragma once

#include "uidevice.h"

namespace sgd {

struct SDLMapping;

class Gamepad : public UIDevice {
public:
	static constexpr uint32_t maxGamepads = 4;

	static constexpr uint32_t numAxes = 6;
	static constexpr uint32_t numButtons = 15;

	static Gamepad* getGamepad(uint32_t id);

	bool connected() const {
		return m_connected;
	}

	String name() const {
		return m_name;
	};

	float axis(uint32_t index) const {
		SGD_ASSERT(index < numAxes);
		return m_axes[index];
	}

	void poll();

private:
	uint32_t m_id;
	bool m_connected{};
	String m_name;

	float m_axes[numAxes]{};

	explicit Gamepad(uint32_t id);

	void onConnected();
	void onDisconnected();
};

} // namespace sgf
