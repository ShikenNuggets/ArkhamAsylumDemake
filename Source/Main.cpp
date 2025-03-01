#include <vector>

#include <dma.h>
#include <dma_tags.h>

#include "Debug.hpp"
#include "GameObject.hpp"
#include "Utils.hpp"
#include "Game/Scene.hpp"
#include "Game/SceneManager.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Renderer.hpp"
#include "Input/Input.hpp"

#include "_Test/TestScene.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

int main(){
	LOG_INFO("Launching PS2Engine...");

	Renderer renderer = Renderer(640, 480);
	renderer.SetClearColor(0, 0, 0);

	SceneManager sceneManager = SceneManager();
	sceneManager.SwitchScene<TestScene>();

	Input::Init();

	while(true){
		Input::Update();

		auto* currentScene = sceneManager.CurrentScene();
		if(currentScene == nullptr){
			continue;
		}

		currentScene->Update();
		renderer.Render(currentScene->GetCamera(), currentScene->GetGameObjects());
	}
	
	return 0;
}