#ifndef PS2E_SCENE_HPP
#define PS2E_SCENE_HPP

#include <vector>

#include "GameObject.hpp"
#include "Graphics/Camera.hpp"

class Scene{
public:
	Scene();
	~Scene();

	GameObject* AddObject();
	GameObject* AddObject(GameObject* gameObject);

	Camera& GetCamera(){ return camera; }
	std::vector<GameObject*>& GetGameObjects(){ return gameObjects; }

private:
	Camera camera;
	std::vector<GameObject*> gameObjects;
};

#endif //!PS2E_SCENE_HPP