#include "Renderer.hpp"

#include <cstring>

#include <dma.h>
#include <dma_tags.h>
#include <draw.h>
#include <draw_buffers.h>
#include <graph.h>
#include <graph_vram.h>
#include <packet.h>

#include "Graphics/ScreenQuad.hpp"

ScreenQuad* sq; // TODO - For testing only, plz fix this

Renderer::Renderer(unsigned int width_, unsigned int height_) : frameBuffer(width_, height_), depthBuffer(width_, height_), width(width_), height(height_), packets{ nullptr, nullptr }, packetCtx(0){
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	
	packet_t* packet = packet_init(16, PACKET_NORMAL);

	// This is our generic qword pointer.
	qword_t* q = packet->data;

	// This will setup a default drawing environment.
	q = draw_setup_environment(q, 0, frameBuffer.Get(), depthBuffer.Get());

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	q = draw_primitive_xyoffset(q, 0, Renderer::gOffsetX - (width / 2.0f), Renderer::gOffsetY - (height / 2.0f));

	// Finish setting up the environment.
	q = draw_finish(q);

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_normal(DMA_CHANNEL_GIF, packet->data, q - packet->data, 0, 0);
	dma_wait_fast();

	packet_free(packet);

	// Two packets to allow double-buffered DMA sends
	packets[0] = packet_init(1024, PACKET_NORMAL);
	packets[1] = packet_init(1024, PACKET_NORMAL);

	create_view_screen(viewScreenMatrix, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);

	dma_wait_fast(); // TODO - is this needed?

	sq = new ScreenQuad();
}

Renderer::~Renderer(){
	packet_free(packets[0]);
	packet_free(packets[1]);

	delete sq;
}

void Renderer::Render(Camera& camera, const std::vector<GameObject*>& gameObjects){
	packet_t* current = packets[packetCtx];

	// Create the local_world matrix.
	MATRIX local_world;
	MATRIX local_screen;
	MATRIX world_view;
	camera.GetWorldView(world_view);

	for(auto* go : gameObjects){
		go->GetLocalWorld(local_world);
		create_local_screen(local_screen, local_world, world_view, viewScreenMatrix);
		go->GetMesh().Update(local_screen);
	}

	// Now grab our qword pointer and increment past the dmatag.
	qword_t* q = current->data;

	uint8_t clearRToUse = clearR;
	uint8_t clearGToUse = clearR;
	uint8_t clearBToUse = clearR;

	if(sq != nullptr){
		clearRToUse = 0x0;
		clearGToUse = 0x0;
		clearBToUse = 0x0;
	}

	// Clear framebuffer but don't update zbuffer.
	q = draw_disable_tests(q, 0, depthBuffer.Get());
	q = draw_clear(q, 0, Renderer::gOffsetX - (width / 2.0f), Renderer::gOffsetY - (height / 2.0f), width, height, clearRToUse, clearGToUse, clearBToUse);
	q = draw_enable_tests(q, 0, depthBuffer.Get());

	if(sq == nullptr){
		for(auto* go : gameObjects){
			q = go->GetMesh().Render(q);
		}
	}else{
		q = sq->Render(q);
	}

	// Setup a finish event.
	q = draw_finish(q);

	// Now send our current dma chain.
	dma_wait_fast();
	dma_channel_send_normal(DMA_CHANNEL_GIF, current->data, q - current->data, 0, 0);

	// Now switch our packets so we can process data while the DMAC is working.
	packetCtx ^= 1;

	// Wait for scene to finish drawing
	draw_wait_finish();
	graph_wait_vsync();
}