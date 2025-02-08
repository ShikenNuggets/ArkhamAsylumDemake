#ifndef PS2E_GAME_OBJECT_HPP
#define PS2E_GAME_OBJECT_HPP

#include <math3d.h>

#include "Game/LogicComponent.hpp"
#include "Graphics/MeshComponent.hpp"

class GameObject{
public:
	GameObject(float xPos = 0.0f, float yPos = 0.0f, float zPos = 0.0f, float xRot = 0.0f, float yRot = 0.0f, float zRot = 0.0f);
	~GameObject();

	void Update();

	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);

	void GetLocalWorld(MATRIX& mat); //TODO - This should be const

	const MeshComponent& GetMesh() const{ return mesh; }
	MeshComponent& GetMesh(){ return mesh; }

private:
	VECTOR position;
	VECTOR rotation;
	MeshComponent mesh;
	LogicComponent logic;
};

#endif //!PS2E_GAME_OBJECT_HPP