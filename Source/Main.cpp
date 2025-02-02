#include <dma.h>
#include <dma_tags.h>

#include "GameObject.hpp"
#include "Utils.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Renderer.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

int main(){
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	Renderer renderer = Renderer(640, 480);

	GameObject object;
	GameObject object2;
	Camera camera;

	object.Move(-16.0f, 0.0f, 0.0f);
	object2.Move(16.0f, 0.0f, 0.0f);

	dma_wait_fast();

	while(true){
		object.Rotate(0.008f, 0.012f, 0.0f);
		object2.Rotate(-0.007f, 0.013f, 0.0f);
		renderer.Render(camera, object, object2);
	}
	
	return 0;
}