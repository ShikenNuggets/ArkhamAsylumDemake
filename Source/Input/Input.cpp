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
	if(SifLoadModule("rom0:SIO2MAN", 0, nullptr) < 0){
		FATAL_ERROR("Failed to load module \"rom0:SIO2MAN\" required for gamepad input!");
	}

	if(SifLoadModule("rom0:PADMAN", 0, nullptr) < 0){
		FATAL_ERROR("Failed to load module \"rom0:PADMAN\" required for gamepad input!");
	}

	if(!padInit(0)){
		FATAL_ERROR("Failed to initialize gamepad library!");
	}

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
}

static bool GetPortAndSlotForPlayerIdx(int& outPort, int& outSlot, int playerIdx){
	BASIC_ASSERT(playerIdx >= 0 && playerIdx <= 8);
	if(playerIdx < 0 || playerIdx > 8){
		return false;
	}

	int playersAssociated = 0;
	for(const auto& port : ports){
		int slots = port.GetNumSlots();
		if(playerIdx > playersAssociated + slots){
			playersAssociated += slots;
			continue;
		}

		outPort = port.GetPort();
		outSlot = playerIdx - playersAssociated;
		return true;
	}

	return false;
}

bool Input::ButtonDown(uint32_t button, int player){
	if(player < 0){
		for(const auto& port : ports){
			if(port.ButtonDown(button)){
				return true;
			}
		}

		return false;
	}

	BASIC_ASSERT(player <= 8);
	if(player > 8){
		return false;
	}

	int port = 0;
	int slot = 0;
	if(!GetPortAndSlotForPlayerIdx(port, slot, player)){
		return false;
	}

	return ports[port].ButtonDown(button, slot);
}

bool Input::ButtonUp(uint32_t button, int player){
	if(player < 0){
		for(const auto& port : ports){
			if(port.ButtonUp(button)){
				return true;
			}
		}

		return false;
	}

	BASIC_ASSERT(player <= 8);
	if(player > 8){
		return false;
	}

	int port = 0;
	int slot = 0;
	if(!GetPortAndSlotForPlayerIdx(port, slot, player)){
		return false;
	}

	return ports[port].ButtonUp(button, slot);
}

bool Input::ButtonHeld(uint32_t button, int player){
	if(player < 0){
		for(const auto& port : ports){
			if(port.ButtonHeld(button)){
				return true;
			}
		}

		return false;
	}

	BASIC_ASSERT(player <= 8);
	if(player > 8){
		return false;
	}

	int port = 0;
	int slot = 0;
	if(!GetPortAndSlotForPlayerIdx(port, slot, player)){
		return false;
	}

	return ports[port].ButtonHeld(button, slot);
}
