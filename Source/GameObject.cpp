#include "GameObject.hpp"

GameObject::GameObject(float xPos, float yPos, float zPos, float xRot, float yRot, float zRot) : position{ xPos, yPos, zPos, 0.0f }, rotation{ xRot, yRot, zRot, 0.0f }, mesh(), logic(this){}

GameObject::~GameObject() = default;

void GameObject::Update(){
	logic.Update();
}

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