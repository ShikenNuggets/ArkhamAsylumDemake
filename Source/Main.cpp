#include <vector>

#include <dma.h>
#include <dma_tags.h>

#include "Debug.hpp"
#include "GameObject.hpp"
#include "Utils.hpp"
#include "Game/Scene.hpp"
#include "Game/SceneManager.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/MoviePlayer.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/TextureBuffer.hpp"
#include "Input/Input.hpp"

#include "Assets/flowerTexture.h"
#include "_Test/TestScene.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

int main(){
	LOG_INFO("Launching PS2Engine...");

	Renderer renderer = Renderer(640, 480);
	renderer.SetClearColor(32, 32, 32);

	TextureBuffer tBuf = TextureBuffer(256, 256, flowerTexture);

	SceneManager sceneManager = SceneManager();
	sceneManager.SwitchScene<TestScene>();

	Input::Init();

	MoviePlayer moviePlayer("ASYLUM_01", 2779);

	while(true){
		Input::Update();

		auto* currentScene = sceneManager.CurrentScene();
		if(currentScene == nullptr){
			continue;
		}

		moviePlayer.Update();

		currentScene->Update();
		renderer.Render(currentScene->GetCamera(), currentScene->GetGameObjects());
	}
	
	return 0;
}