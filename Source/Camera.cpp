#include "Camera.hpp"

Camera::Camera() : position{ 0.0f, 0.0f, 100.0f, 1.0f }, rotation{ 0.0f, 0.0f, 0.0f, 1.0f }{}

Camera::~Camera() = default;

void Camera::GetWorldView(MATRIX& mat){
	create_world_view(mat, position, rotation);
}