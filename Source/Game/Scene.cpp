#include "Scene.hpp"

#include "Debug.hpp"

Scene::Scene() : camera(), gameObjects(){}

Scene::~Scene(){
	for(auto* go : gameObjects){
		delete go;
	}
}

GameObject* Scene::AddObject(float xPos, float yPos, float zPos, float xRot, float yRot, float zRot){
	GameObject* go = new GameObject(xPos, yPos, zPos, xRot, yRot, zRot);
	gameObjects.push_back(go);
	return go;
}

GameObject* Scene::AddObject(GameObject* gameObject){
	if(gameObject == nullptr){
		LOG_WARNING("Tried to add null GameObject to the scene!");
		return nullptr;
	}

	gameObjects.push_back(gameObject);
	return gameObject;
}
