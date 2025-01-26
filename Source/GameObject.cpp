#include "GameObject.hpp"

GameObject::GameObject() : position{ 0.0f, 0.0f, 0.0f, 0.0f }, rotation{ 0.0f, 0.0f, 0.0f, 0.0f }, mesh(){}

GameObject::~GameObject(){}

void GameObject::Move(float x, float y, float z){
	position[0] += x;
	position[1] += y;
	position[2] += z;
}

void GameObject::Rotate(float x, float y, float z){
	rotation[0] += x;
	rotation[1] += y;
	rotation[2] += z;
}

void GameObject::GetLocalWorld(MATRIX& mat){
	create_local_world(mat, position, rotation);
}