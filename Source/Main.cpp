#include <vector>

#include <dma.h>
#include <dma_tags.h>

#include "Debug.hpp"
#include "GameObject.hpp"
#include "Utils.hpp"
#include "Game/Scene.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Renderer.hpp"
#include "Input/Input.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

int main(){
	LOG_INFO("Launching PS2Engine...");
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	Renderer renderer = Renderer(640, 480);
	renderer.SetClearColor(0, 0, 0);

	Scene currentScene;

	currentScene.AddObject(-16.0f, 16.0f, 0.0f);
	currentScene.AddObject(16.0f, 16.0f, 0.0f);
	currentScene.AddObject(-16.0f, -16.0f, 0.0f);
	currentScene.AddObject(16.0f, -16.0f, 0.0f);

	dma_wait_fast();

	Input::Init();

	while(true){
		Input::Update();

		currentScene.Update();
		renderer.Render(currentScene.GetCamera(), currentScene.GetGameObjects());
	}
	
	return 0;
}