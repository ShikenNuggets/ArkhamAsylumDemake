#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include <draw.h>
#include <draw2d.h>
#include <draw_types.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <dma.h>
#include <dma_tags.h>
#include <packet.h>

#include "Camera.hpp"
#include "DepthBuffer.hpp"
#include "FrameBuffer.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Utils.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

void* __gxx_personality_v0; // Apparently this needs to be defined somewhere due to exception handling being disabled

int Render(FrameBuffer& frame, DepthBuffer& depth){
	// The data packets for double buffering dma sends.
	packet_t* packets[2];
	packet_t* current;

	packets[0] = packet_init(100, PACKET_NORMAL);
	packets[1] = packet_init(100, PACKET_NORMAL);

	GameObject object;
	GameObject object2;
	Camera camera;

	object.Move(-16.0f, 0.0f, 0.0f);
	object2.Move(16.0f, 0.0f, 0.0f);

	// Create the view_screen matrix.
	MATRIX view_screen;
	create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);

	// Wait for any previous dma transfers to finish before starting.
	dma_wait_fast();

	// The main loop...
	int context = 0;
	while(true){
		qword_t* q = nullptr;

		current = packets[context];

		// Spin the cube a bit.
		object.Rotate(0.008f, 0.012f, 0.0f);
		object2.Rotate(-0.007f, 0.013f, 0.0f);

		// Create the local_world matrix.
		MATRIX local_world;
		object.GetLocalWorld(local_world);

		// Create the world_view matrix.
		MATRIX world_view;
		camera.GetWorldView(world_view);

		// Create the local_screen matrix.
		MATRIX local_screen;
		create_local_screen(local_screen, local_world, world_view, view_screen);
		
		object.GetMesh().Update(local_screen);

		object2.GetLocalWorld(local_world);
		create_local_screen(local_screen, local_world, world_view, view_screen);
		object2.GetMesh().Update(local_screen);

		// Grab our dmatag pointer for the dma chain.
		qword_t* dmatag = current->data;

		// Now grab our qword pointer and increment past the dmatag.
		q = dmatag;
		q++;

		// Clear framebuffer but don't update zbuffer.
		q = draw_disable_tests(q, 0, depth.Get());
		q = draw_clear(q, 0, Renderer::gOffsetX - (gScreenWidth / 2.0f), Renderer::gOffsetY - (gScreenHeight / 2.0f), gScreenWidth, gScreenHeight, 0x0, 0x0, 0x0);
		q = draw_enable_tests(q, 0, depth.Get());

		q = object.GetMesh().Render(q);
		q = object2.GetMesh().Render(q);

		// Setup a finish event.
		q = draw_finish(q);

		// Define our dmatag for the dma chain.
		DMATAG_END(dmatag, (q - current->data) - 1, 0, 0, 0);

		// Now send our current dma chain.
		dma_wait_fast();
		dma_channel_send_chain(DMA_CHANNEL_GIF, current->data, q - current->data, 0, 0);

		// Now switch our packets so we can process data while the DMAC is working.
		context ^= 1;

		// Wait for scene to finish drawing
		draw_wait_finish();

		graph_wait_vsync();
	}

	packet_free(packets[0]);
	packet_free(packets[1]);

	return 0;
}

int main(){
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	Renderer renderer = Renderer(640, 480);

	Render(renderer.GetFrameBuffer(), renderer.GetDepthBuffer());
	return 0;
}