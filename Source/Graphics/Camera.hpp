#ifndef PS2E_CAMERA_HPP
#define PS2E_CAMERA_HPP

#include <math3d.h>

#include "Debug.hpp"
#include "Time.hpp"
#include "Input/Input.hpp"

class Camera{
public:
	Camera();
	~Camera();

	// TODO - This is just for testing
	void Update();

	void GetWorldView(MATRIX& mat); //TODO - This should be const

private:
	VECTOR position;
	VECTOR rotation;
};

#endif //!PS2E_CAMERA_HPP