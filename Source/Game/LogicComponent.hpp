#ifndef PS2E_LOGIC_COMPONENT_H
#define PS2E_LOGIC_COMPONENT_H

#include "Component.hpp"

class LogicComponent : public Component{
public:
	LogicComponent(GameObject* parent_);

	void Update();

private:
	float rotX;
	float rotY;
};

#endif //!PS2E_LOGIC_COMPONENT_H