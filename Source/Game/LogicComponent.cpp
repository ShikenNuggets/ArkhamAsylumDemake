#include "LogicComponent.hpp"

#include <cstdlib>

#include "Debug.hpp"
#include "GameObject.hpp"
#include "Time.hpp"

LogicComponent::LogicComponent(GameObject* parent_) : Component(parent_){
	constexpr float minX = -1.0f;
	constexpr float maxX =  1.0f;

	constexpr float minY = -1.0f;
	constexpr float maxY =  1.0f;

	rotX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
	rotY = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
}

void LogicComponent::Update(){
	parent->Rotate(rotX * Time::DeltaTime() * Time::timeScale, rotY * Time::DeltaTime() * Time::timeScale, 0.0f);
}