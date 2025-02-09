#ifndef PS2E_GAMEPAD_HPP
#define PS2E_GAMEPAD_HPP

#include <cstdint>

enum class ButtonID : uint8_t{
	Triangle,
	Circle,
	Square,
	Cross,
	L1,
	R1,
	L2,
	R2,
	L3,
	R3,
	Up,
	Down,
	Left,
	Right,

	ButtonID_MAX
};

enum class AxisID : uint8_t{
	LeftX,
	LeftY,
	RightX,
	RightY,

	AxisID_MAX
};

class Gamepad{
public:
	Gamepad(int port_, int slot_);

	void Update();

	bool ButtonDown(uint32_t button);
	bool ButtonUp(uint32_t button);
	bool ButtonHeld(uint32_t button);

private:
	char padBuffer[256] __attribute__((aligned(64)));
	char actAlign[6];
	int port;
	int slot;
	uint32_t prevState;
	uint32_t curState;

	void WaitUntilReady();
};

#endif //!PS2E_GAMEPAD_HPP