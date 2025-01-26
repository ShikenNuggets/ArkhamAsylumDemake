#ifndef PS2E_GAME_OBJECT_HPP
#define PS2E_GAME_OBJECT_HPP

#include <math3d.h>

#include "MeshComponent.hpp"

class GameObject{
public:
	GameObject();
	~GameObject();

	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);

	void GetLocalWorld(MATRIX& mat);

	const MeshComponent& GetMesh() const{ return mesh; }
	MeshComponent& GetMesh(){ return mesh; }

private:
	VECTOR position;
	VECTOR rotation;
	MeshComponent mesh;
};

#endif //!PS2E_GAME_OBJECT_HPP