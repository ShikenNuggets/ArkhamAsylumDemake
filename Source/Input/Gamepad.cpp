#include "Gamepad.hpp"

#include <bitset>

#include <libpad.h>
#include <loadfile.h>
#include <kernel.h>

#include "Debug.hpp"

Gamepad::Gamepad(int port_, int slot_) : padBuffer(), actAlign(), port(port_), slot(slot_), prevState(0), curState(0), isSetup(false){
	if(!padPortOpen(port, slot, padBuffer)){
		FATAL_ERROR("Could not open Gamepad port [%d, %d]", port, slot);
	}

	SetupOnConnected();
}

Gamepad::~Gamepad(){
	if(!padPortClose(port, slot)){
		LOG_ERROR("An error occured while closing Gamepad port [%d, %d]", port, slot);
	}
}

void Gamepad::Update(){
	if(!TryWaitUntilReady()){
		return;
	}
	
	if(!isSetup){
		SetupOnConnected();
	}

	padButtonStatus buttons;
	int result = padRead(port, slot, &buttons);
	if(result != 0){
		prevState = curState;
		curState = 0xffff ^ buttons.btns;

		axisState[JoyAxis::LeftX] = buttons.ljoy_h;
		axisState[JoyAxis::LeftY] = buttons.ljoy_v;
		axisState[JoyAxis::RightX] = buttons.rjoy_h;
		axisState[JoyAxis::RightY] = buttons.rjoy_v;
	}
}

bool Gamepad::ButtonDown(uint32_t button) const{
	ASSERT(IsValidButton(button), "Assert Failed: [%d] is not a valid button!", button);
	return isSetup && (curState & button) && !(prevState & button);
}

bool Gamepad::ButtonUp(uint32_t button) const{
	ASSERT(IsValidButton(button), "Assert Failed: [%d] is not a valid button!", button);
	return isSetup && !(curState & button) && (prevState & button);
}

bool Gamepad::ButtonHeld(uint32_t button) const{
	ASSERT(IsValidButton(button), "Assert Failed: [%d] is not a valid button!", button);
	return isSetup && (curState & button);
}

float Gamepad::GetAxis(JoyAxis axis) const{
	BASIC_ASSERT(axis < JoyAxis::JoyAxis_MAX);
	if(!isSetup || axis >= JoyAxis::JoyAxis_MAX){
		return 0.0f;
	}

	const float value = axisState[axis];
	constexpr float maxValue = std::numeric_limits<unsigned char>::max();
	float result = ((value / maxValue) * 2.0f) - 1.0f;

	if(axis == JoyAxis::LeftY || axis == JoyAxis::RightY){
		result = -result;
	}

	// TODO - Smooth out remaining range instead of just cutting off at the deadzone
	// TODO - Configurable deadzone
	if(result > -0.25f && result < 0.25f){
		return 0.0f;
	}

	if(result < -1.0f){
		return -1.0f;
	}else if(result > 1.0f){
		return 1.0f;
	}

	return result;
}

void Gamepad::SetupOnConnected(){
	if(!TryWaitUntilReady() || isSetup){
		return;
	}

	int modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
	if(modes == 0){
		LOG_VERBOSE("Gamepad has no actuator engines");
		isSetup = true;
		return;
	}

	bool hasDualShockMode = false;
	for(int i = 0; i < modes; i++){
		if(padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK){
			hasDualShockMode = true;
			break;
		}
	}

	if(!hasDualShockMode){
		LOG_VERBOSE("Gamepad has no Dualshock mode");
		isSetup = true;
		return;
	}

	// If ExId != 0x0 => This controller has actuator engines
	// This check should always pass if the Dual Shock test above passed
	int ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
	if(ret == 0){
		LOG_WARNING("Gamepad has a Dualshock mode, but ExId returned 0!");
		isSetup = true;
		return;
	}

	LOG_VERBOSE("Enabling Dualshock Functions on Gamepad [%d,%d]", port, slot);

	// When using MMODE_LOCK, user can't change mode with Select button
	if(!padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK)){
		LOG_ERROR("Could not set pad to locked Dualshock mode!");
		return;
	}

	if(!TryWaitUntilReady()){
		return;
	}

	unsigned char actuators = padInfoAct(port, slot, -1, 0);
	if(actuators != 0){
		actAlign[0] = 0;   // Enable small engine
		actAlign[1] = 1;   // Enable big engine
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		if(!TryWaitUntilReady()){
			return;
		}

		if(!padSetActAlign(0, 0, actAlign)){
			LOG_ERROR("Could not initialize gamepad actuators!");
		}
    }

	isSetup = true;
}

bool Gamepad::TryWaitUntilReady(int numRetries){
	int lastState = -1;
	for(int i = 0; i < numRetries; i++){
		int state = padGetState(port, slot);
		if(state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1){
			return true;
		}

		if(state == PAD_STATE_DISCONN || state == PAD_STATE_ERROR){
			isSetup = false;
			return false;
		}

		if(state != lastState){
			char stateString[16];
			padStateInt2String(state, stateString);
			LOG_VERBOSE("Please wait, Gamepad [%d,%d] is in state %s", port, slot, stateString);
		}
		lastState = state;
	}

	LOG_VERBOSE("Gamepad not in a usable state, will try again on next Input update");
	return false;
}

bool Gamepad::IsValidButton(uint32_t button){
	switch(button){
		case PAD_L1:		[[fallthrough]];
		case PAD_L2:		[[fallthrough]];
		case PAD_L3:		[[fallthrough]];
		case PAD_R1:		[[fallthrough]];
		case PAD_R2:		[[fallthrough]];
		case PAD_R3:		[[fallthrough]];
		case PAD_TRIANGLE:	[[fallthrough]];
		case PAD_CIRCLE:	[[fallthrough]];
		case PAD_SQUARE:	[[fallthrough]];
		case PAD_CROSS:		[[fallthrough]];
		case PAD_UP:		[[fallthrough]];
		case PAD_DOWN:		[[fallthrough]];
		case PAD_LEFT:		[[fallthrough]];
		case PAD_RIGHT:		[[fallthrough]];
		case PAD_START:		[[fallthrough]];
		case PAD_SELECT:
			return true;
		default:
			return false;
	}
}
