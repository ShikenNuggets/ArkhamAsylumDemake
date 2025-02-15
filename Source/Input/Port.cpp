#include "Port.hpp"

#include <libpad.h>

#include "Debug.hpp"

Port::Port(int port_) : portNum(port_){
	const int maxSlots = padGetSlotMax(portNum);
	if(maxSlots <= 0){
		LOG_ERROR("Port %d has an invalid number of slots (%d)!", portNum, maxSlots);
	}

	slots.reserve(4);
	for(int i = 0; i < maxSlots; i++){
		slots.emplace_back(portNum, i); // Using emplace_back instead of push_back is VERY important since this represents a hardware device
	}
}

void Port::Update(){
	for(auto& gamepad : slots){
		gamepad.Update();
	}
}

bool Port::ButtonDown(uint32_t button, int slot_) const{
	if(slot_ < 0){
		for(const auto& gamepad : slots){
			if(gamepad.ButtonDown(button)){
				return true;
			}
		}

		return false;
	}

	if(static_cast<size_t>(slot_) >= slots.size()){
		LOG_ERROR("ButtonDown called with an invalid slot (%d)!", slot_);
		return false;
	}

	return slots[slot_].ButtonDown(button);
}

bool Port::ButtonUp(uint32_t button, int slot_) const{
	if(slot_ < 0){
		for(const auto& gamepad : slots){
			if(gamepad.ButtonUp(button)){
				return true;
			}
		}

		return false;
	}

	if(static_cast<size_t>(slot_) >= slots.size()){
		LOG_ERROR("ButtonUp called with an invalid slot (%d)!", slot_);
		return false;
	}

	return slots[slot_].ButtonUp(button);
}

bool Port::ButtonHeld(uint32_t button, int slot_) const{
	if(slot_ < 0){
		for(const auto& gamepad : slots){
			if(gamepad.ButtonHeld(button)){
				return true;
			}
		}

		return false;
	}

	if(static_cast<size_t>(slot_) >= slots.size()){
		LOG_ERROR("ButtonHeld called with an invalid slot (%d)!", slot_);
		return false;
	}

	return slots[slot_].ButtonHeld(button);
}

float Port::GetAxis(JoyAxis axis, int slot_) const{
	if(slot_ < 0){
		for(const auto& gamepad : slots){
			const float value = gamepad.GetAxis(axis);
			if(value > 0.01f || value < -0.01f){
				return value;
			}
		}

		return 0.0f;
	}

	if(static_cast<size_t>(slot_) >= slots.size()){
		LOG_ERROR("ButtonHeld called with an invalid slot (%d)!", slot_);
		return 0.0f;
	}

	return slots[slot_].GetAxis(axis);
}
