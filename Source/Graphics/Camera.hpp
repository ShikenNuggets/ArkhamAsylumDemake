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
		
		if(Input::ButtonHeld(PAD_UP)){
			position[1] += frameTime * moveSpeed;
		}else if(Input::ButtonHeld(PAD_DOWN)){
			position[1] -= frameTime * moveSpeed;
		}

		if(Input::ButtonHeld(PAD_LEFT)){
			position[0] -= frameTime * moveSpeed;
		}else if(Input::ButtonHeld(PAD_RIGHT)){
			position[0] += frameTime * moveSpeed;
		}

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