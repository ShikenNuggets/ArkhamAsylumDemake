#ifndef PS2E_CAMERA_HPP
#define PS2E_CAMERA_HPP

#include <math3d.h>

class Camera{
public:
	Camera();
	~Camera();

	VECTOR position;
	VECTOR rotation;
};

#endif //!PS2E_CAMERA_HPP