#ifndef PS2E_INPUT_PORT_HPP
#define PS2E_INPUT_PORT_HPP

#include <cstdint>
#include <vector>

#include "Gamepad.hpp"

class Port{
public:
	Port(int port_);

	void Update();

	// slot < 0 will return true if ANY gamepad has the button down/up/held
	bool ButtonDown(uint32_t button, int slot_ = -1) const; 
	bool ButtonUp(uint32_t button, int slot_ = -1) const;
	bool ButtonHeld(uint32_t button, int slot_ = -1) const;

	int GetPort() const{ return portNum; }
	int GetNumSlots() const{ return slots.size(); }

private:
	int portNum;
	std::vector<Gamepad> slots;
};

#endif //!PS2E_INPUT_PORT_HPP