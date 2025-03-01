#include "Camera.hpp"

Camera::Camera() : position{ 0.0f, 0.0f, 100.0f, 1.0f }, rotation{ 0.0f, 0.0f, 0.0f, 1.0f }{}

Camera::~Camera() = default;

// TODO - This is just for testing
void Camera::Update(){
	constexpr float moveSpeed = 50.0f;
	constexpr float rotateSpeed = 1.0f;

	position[0] += Input::GetAxis(JoyAxis::LeftX) * Time::DeltaTime() * moveSpeed;
	position[1] += Input::GetAxis(JoyAxis::LeftY) * Time::DeltaTime() * moveSpeed;

	rotation[1] += Input::GetAxis(JoyAxis::RightX) * Time::DeltaTime() * rotateSpeed;
	rotation[0] += Input::GetAxis(JoyAxis::RightY) * Time::DeltaTime() * rotateSpeed;

	if(Input::ButtonHeld(PAD_L1)){
		position[2] += Time::DeltaTime() * moveSpeed;
	}else if(Input::ButtonHeld(PAD_R1)){
		position[2] -= Time::DeltaTime() * moveSpeed;
	}

	if(Input::ButtonDown(PAD_CROSS)){
		if(Time::timeScale >= 1.0f){
			Time::timeScale = 0.25f;
		}else{
			Time::timeScale = 1.0f;
		}
	}
}

void Camera::GetWorldView(MATRIX& mat){
	create_world_view(mat, position, rotation);
}