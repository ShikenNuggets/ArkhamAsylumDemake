#include <vector>

#include <dma.h>
#include <dma_tags.h>

#include "Debug.hpp"
#include "GameObject.hpp"
#include "Utils.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Renderer.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

int main(){
	LOG_INFO("Launching PS2Engine...");
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	Renderer renderer = Renderer(640, 480);
	renderer.SetClearColor(0, 0, 0);

	Camera camera;

	std::vector<GameObject*> gameObjects;
	gameObjects.reserve(2);

	GameObject* go = new GameObject();
	go->Move(-16.0f, 0.0f, 0.0f);
	gameObjects.push_back(go);

	go = new GameObject();
	go->Move(16.0f, 0.0f, 0.0f);
	gameObjects.push_back(go);

	dma_wait_fast();

	while(true){
		gameObjects[0]->Rotate(0.008f, 0.012f, 0.0f);
		gameObjects[1]->Rotate(-0.007f, 0.013f, 0.0f);
		renderer.Render(camera, gameObjects);
	}
	
	return 0;
}