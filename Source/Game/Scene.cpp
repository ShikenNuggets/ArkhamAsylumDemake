#include "Scene.hpp"

#include "Debug.hpp"

Scene::Scene() : camera(), gameObjects(){}

Scene::~Scene(){
	for(auto* go : gameObjects){
		delete go;
	}
}

GameObject* Scene::AddObject(){
	GameObject* go = new GameObject();
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
