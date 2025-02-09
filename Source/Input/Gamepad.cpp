#include "Gamepad.hpp"

#include <bitset>

#include <libpad.h>
#include <loadfile.h>
#include <kernel.h>

#include "Debug.hpp"

// TODO - What should the destructor for this class do exactly?
Gamepad::Gamepad(int port_, int slot_) : port(port_), slot(slot_){
	padPortOpen(0, 0, padBuffer);

	WaitUntilReady();

	int modes = padInfoMode(0, 0, PAD_MODETABLE, -1);
	LOG_VERBOSE("Gamepad [%d,%d] has %d modes", port, slot, modes);

    if(modes > 0){
		printf("( ");
		for(int i = 0; i < modes; i++) {
			printf("%d ", padInfoMode(0, 0, PAD_MODETABLE, i));
		}
		printf(")");
	}

	LOG_VERBOSE("Gamepad [%d,%d] currently using mode %d", port, slot, padInfoMode(port, slot, PAD_MODECURID, 0));

	if(modes == 0){
		LOG_VERBOSE("Gamepad has no actuator engines");
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
		return;
	}

	// If ExId != 0x0 => This controller has actuator engines
	// This check should always pass if the Dual Shock test above passed
	int ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
	if(ret == 0){
		LOG_WARNING("Gamepad has a Dualshock mode, but ExId returned 0!");
		return;
	}

	printf("Enabling dual shock functions\n");
	LOG_VERBOSE("Enabling Dualshock Functions on Gamepad [%d,%d]", port, slot);

	// When using MMODE_LOCK, user can't change mode with Select button
	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

	WaitUntilReady();
	LOG_VERBOSE("infoPressMode: %d", padInfoPressMode(port, slot));

	WaitUntilReady();
	LOG_VERBOSE("enterPressMode: %d", padEnterPressMode(port, slot));

	WaitUntilReady();
	unsigned char actuators = padInfoAct(port, slot, -1, 0);
	LOG_VERBOSE("# of actuators: %d", actuators);

	if(actuators != 0){
		actAlign[0] = 0;   // Enable small engine
		actAlign[1] = 1;   // Enable big engine
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		WaitUntilReady();
		LOG_VERBOSE("padSetActAlign: %d", padSetActAlign(0, 0, actAlign));
    }else{
		LOG_VERBOSE("Did not find any actuators");
	}

	WaitUntilReady();
}

void Gamepad::Update(){
	WaitUntilReady();

	padButtonStatus buttons;
	int result = padRead(port, slot, &buttons);
	if(result != 0){
		prevState = curState;
		curState = 0xffff ^ buttons.btns;
	}
}

bool Gamepad::ButtonDown(uint32_t button){
	return curState & button && !(prevState & button);
}

bool Gamepad::ButtonUp(uint32_t button){
	return !(curState & button) && prevState & button;
}

bool Gamepad::ButtonHeld(uint32_t button){
	return curState & button;
}

void Gamepad::WaitUntilReady(){
	char stateString[16];

	int state = padGetState(port, slot);
	int lastState = -1;
	while(state != PAD_STATE_STABLE && state != PAD_STATE_FINDCTP1){
		if(state != lastState){
			padStateInt2String(state, stateString);
			LOG_VERBOSE("Please wait, Gamepad [%d,%d] is in state %s", port, slot, stateString);
		}
		lastState = state;
		state = padGetState(port, slot);
	}
}
