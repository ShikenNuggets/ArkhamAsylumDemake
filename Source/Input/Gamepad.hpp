#ifndef PS2E_GAMEPAD_HPP
#define PS2E_GAMEPAD_HPP

#include <cstdint>
#include <limits>

#include "Debug.hpp"

enum class JoyAxis : uint8_t{
	LeftX,
	LeftY,
	RightX,
	RightY,

	JoyAxis_MAX
};

class AxisState{
public:
	AxisState() : axisState{ neutralValue, neutralValue, neutralValue, neutralValue }{}

	unsigned char operator[](JoyAxis axis) const{
		ASSERT(axis < JoyAxis::JoyAxis_MAX, "Invalid Axis value passed to AxisState[]");
		return axisState[static_cast<uint8_t>(axis)];
	}

	unsigned char& operator[](JoyAxis axis){
		ASSERT(axis < JoyAxis::JoyAxis_MAX, "Invalid Axis value passed to AxisState[]");
		return axisState[static_cast<uint8_t>(axis)];
	}

private:
	static constexpr unsigned char neutralValue = std::numeric_limits<unsigned char>::max() / 2;
	unsigned char axisState[static_cast<uint8_t>(JoyAxis::JoyAxis_MAX)];
};

class Gamepad{
public:
	Gamepad(int port_, int slot_);
	~Gamepad();

	void Update();

	bool ButtonDown(uint32_t button) const;
	bool ButtonUp(uint32_t button) const;
	bool ButtonHeld(uint32_t button) const;

	float GetAxisRaw(JoyAxis axis) const;
	float GetAxis(JoyAxis axis) const;

private:
	char padBuffer[256] __attribute__((aligned(64)));
	char actAlign[6];
	int port;
	int slot;
	uint32_t prevState;
	uint32_t curState;
	AxisState axisState;
	bool isSetup;

	void SetupOnConnected();
	bool TryWaitUntilReady(int numRetries = std::numeric_limits<int>::max());

	static bool IsValidButton(uint32_t button);
};

#endif //!PS2E_GAMEPAD_HPP