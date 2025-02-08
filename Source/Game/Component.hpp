#ifndef PS2E_COMPONENT_HPP
#define PS2E_COMPONENT_HPP

class GameObject;

class Component{
public:
	Component(GameObject* parent_) : parent(parent_){}
	virtual ~Component(){}

protected:
	GameObject* parent;
};

#endif //!PS2E_COMPONENT_HPP