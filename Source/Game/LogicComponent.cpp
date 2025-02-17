#include "LogicComponent.hpp"

#include <cstdlib>

#include "Debug.hpp"
#include "GameObject.hpp"

LogicComponent::LogicComponent(GameObject* parent_) : Component(parent_){
	constexpr float minX = -0.02;
	constexpr float maxX =  0.02;

	constexpr float minY = -0.02f;
	constexpr float maxY =  0.02f;

	rotX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
	rotY = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
}

void LogicComponent::Update(){
	parent->Rotate(rotX, rotY, 0.0f);
}