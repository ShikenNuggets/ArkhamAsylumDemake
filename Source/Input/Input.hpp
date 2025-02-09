#ifndef PS2E_INPUT_HPP
#define PS2E_INPUT_HPP

#include <cstdint>

#include <libpad.h>

namespace Input{
	void Init();
	void Update();

	bool ButtonDown(uint32_t button, int player = -1);
	bool ButtonUp(uint32_t button, int player = -1);
	bool ButtonHeld(uint32_t button, int player = -1);
};

#endif //!PS2E_INPUT_HPP