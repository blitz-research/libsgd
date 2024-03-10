#pragma once

#include "uidevice.h"

namespace sgd {

SGD_SHARED(Window);

struct SDLMapping;

class Gamepad : public UIDevice {
public:
	static constexpr uint32_t maxGamepads = 4;

	static constexpr uint32_t numAxes = 6;
	static constexpr uint32_t numButtons = 15;

	String name() const {
		return m_name;
	};

	bool connected() const {
		return m_connected;
	}

	float axis(uint32_t index) const {
		SGD_ASSERT(index < numAxes);
		return m_axes[index];
	}

	static Gamepad* getGamepad(uint32_t id);

	static void createGamepads();

	static void updateGamepads();

private:
	friend class Window;

	uint32_t m_id;
	bool m_connected{};
	String m_name;
	float m_axes[numAxes]{};

	Gamepad(uint32_t id);

	void onConnected();
	void onDisconnected();

	void onPoll() override;
};

} // namespace sgf
