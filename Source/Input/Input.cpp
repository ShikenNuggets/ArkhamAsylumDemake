#include "Input.hpp"

#include <cstdio>
#include <vector>

#include <libpad.h>
#include <loadfile.h>
#include <sifrpc.h>

#include "Debug.hpp"
#include "Gamepad.hpp"
#include "Port.hpp"

int gMaxPorts = 0;
std::vector<int> gSlotMax;

std::vector<Port> ports;

void Input::Init(){
	// Load Modules
	SifLoadModule("rom0:SIO2MAN", 0, nullptr); // TODO - Error handling
	SifLoadModule("rom0:PADMAN", 0, nullptr); // TODO - Error handling

	padInit(0); // TODO - Error handling

	const int maxPorts = padGetPortMax();
	ports.reserve(maxPorts);
	for(int i = 0; i < maxPorts; i++){
		ports.emplace_back(i);
	}
}

void Input::Update(){
	for(auto& port : ports){
		port.Update();
	}

	if(ports[0].ButtonDown(PAD_TRIANGLE, -1)){
		LOG_VERBOSE("Triangle down!");
	}

	if(ports[0].ButtonUp(PAD_TRIANGLE, -1)){
		LOG_VERBOSE("Triangle up!");
	}
}