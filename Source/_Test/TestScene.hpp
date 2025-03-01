#ifndef PS2E_TEST_TEST_SCENE_HPP
#define PS2E_TEST_TEST_SCENE_HPP

#include "Game/Scene.hpp"

class TestScene : public Scene{
public:
	TestScene(){
		AddObject(-16.0f, 16.0f, 0.0f);
		AddObject(16.0f, 16.0f, 0.0f);
		AddObject(-16.0f, -16.0f, 0.0f);
		AddObject(16.0f, -16.0f, 0.0f);
	}

	virtual ~TestScene() override = default;
};

#endif //!PS2E_TEST_TEST_SCENE_HPP