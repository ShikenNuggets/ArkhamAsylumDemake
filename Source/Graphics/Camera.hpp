#ifndef PS2E_CAMERA_HPP
#define PS2E_CAMERA_HPP

#include <math3d.h>

#include "Debug.hpp"
#include "Input/Input.hpp"

class Camera{
public:
	Camera();
	~Camera();

	// TODO - This is just for testing
	inline void Update(){
		constexpr float moveSpeed = 50.0f;
		constexpr float frameTime = 1.0f / 60.0f;

		position[0] += Input::GetAxis(JoyAxis::LeftX) * frameTime * moveSpeed;
		position[1] += Input::GetAxis(JoyAxis::LeftY) * frameTime * moveSpeed;
		
		if(Input::ButtonHeld(PAD_L1)){
			position[2] += frameTime * moveSpeed;
		}else if(Input::ButtonHeld(PAD_R1)){
			position[2] -= frameTime * moveSpeed;
		}
	}

	void GetWorldView(MATRIX& mat); //TODO - This should be const

private:
	VECTOR position;
	VECTOR rotation;
};

#endif //!PS2E_CAMERA_HPP