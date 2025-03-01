#ifndef PS2E_SCENE_MANAGER_HPP
#define PS2E_SCENE_MANAGER_HPP

#include <memory>

#include "Game/Scene.hpp"

class SceneManager{
public:
	SceneManager() : current(nullptr){}
	~SceneManager() = default;

	Scene* CurrentScene(){ return current.get(); }

	template <class T>
	void SwitchScene(){ current = std::make_unique<T>(); }

	template<class T, class... V>
	void SwitchScene(V... args){ current = std::make_unique<T>(args...); }

private:
	std::unique_ptr<Scene> current;
};

#endif //!PS2E_SCENE_MANAGER_HPP