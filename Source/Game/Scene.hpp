#ifndef PS2E_SCENE_HPP
#define PS2E_SCENE_HPP

#include <vector>

#include "GameObject.hpp"
#include "Graphics/Camera.hpp"

class Scene{
public:
	Scene();
	~Scene();

	void Update();

	GameObject* AddObject(float xPos = 0.0f, float yPos = 0.0f, float zPos = 0.0f, float xRot = 0.0f, float yRot = 0.0f, float zRot = 0.0f);
	GameObject* AddObject(GameObject* gameObject);

	Camera& GetCamera(){ return camera; }
	std::vector<GameObject*>& GetGameObjects(){ return gameObjects; }

private:
	Camera camera;
	std::vector<GameObject*> gameObjects;
};

#endif //!PS2E_SCENE_HPP