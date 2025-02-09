#ifndef PS2E_GAMEPAD_HPP
#define PS2E_GAMEPAD_HPP

#include <cstdint>
#include <limits>

class Gamepad{
public:
	Gamepad(int port_, int slot_);
	~Gamepad();

	void Update();

	bool ButtonDown(uint32_t button) const;
	bool ButtonUp(uint32_t button) const;
	bool ButtonHeld(uint32_t button) const;

private:
	char padBuffer[256] __attribute__((aligned(64)));
	char actAlign[6];
	int port;
	int slot;
	uint32_t prevState;
	uint32_t curState;
	bool isSetup;

	void SetupOnConnected();
	bool TryWaitUntilReady(int numRetries = std::numeric_limits<int>::max());

	static bool IsValidButton(uint32_t button);
};

#endif //!PS2E_GAMEPAD_HPP