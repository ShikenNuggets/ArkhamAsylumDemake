#include "Input.hpp"

#include <cstdio>
#include <vector>

#include <libpad.h>
#include <loadfile.h>
#include <sifrpc.h>

#include "Debug.hpp"
#include "Gamepad.hpp"

int gMaxPorts = 0;
std::vector<int> gSlotMax;

Gamepad* gGamepad1; // TODO - We should have some kind of port/slot structure that owns this rather than it just sitting here

void Input::Init(){
	// Load Modules
	SifLoadModule("rom0:SIO2MAN", 0, nullptr); // TODO - Error handling
	SifLoadModule("rom0:PADMAN", 0, nullptr); // TODO - Error handling

	padInit(0);

	int maxPorts = padGetPortMax();
	gSlotMax.reserve(maxPorts);
	for(int i = 0; i < maxPorts; i++){
		gSlotMax.push_back(padGetSlotMax(i));
	}

	gGamepad1 = new Gamepad(0, 0); // TODO - Remember to delete this somewhere
}

void Input::Update(){
	if(gGamepad1 != nullptr){
		gGamepad1->Update();

		// Just for testing
		if(gGamepad1->ButtonDown(PAD_TRIANGLE)){
			LOG_VERBOSE("Triangle down!");
		}

		if(gGamepad1->ButtonUp(PAD_TRIANGLE)){
			LOG_VERBOSE("Triangle up!");
		}
	}
}