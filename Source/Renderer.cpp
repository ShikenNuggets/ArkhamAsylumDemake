#include "Renderer.hpp"

#include <cstring>
#include <stdlib.h>

#include <dma.h>
#include <draw.h>
#include <draw_buffers.h>
#include <graph.h>
#include <graph_vram.h>
#include <packet.h>

qword_t* DrawState::gCmdBuffer = nullptr;

Renderer::Renderer(unsigned int width_, unsigned int height_) : frameBuffer(width_, height_), depthBuffer(width_, height_), width(width_), height(height_){
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
}

Renderer::~Renderer(){}

int Renderer::gs_init(DrawState& ds, int psm, int psmz){
	if(DrawState::gCmdBuffer == nullptr){
		DrawState::gCmdBuffer = static_cast<qword_t*>(std::malloc(DrawState::gCmdBufferSize)); // TODO - RAII, custom memory allocator
	}

	ds.frameBuffer.address = graph_vram_allocate(ds.width, ds.height, psm, GRAPH_ALIGN_PAGE);
	ds.frameBuffer.width = ds.width;
	ds.frameBuffer.height = ds.height;
	ds.frameBuffer.psm = psm;
	ds.frameBuffer.mask = 0;

	ds.zBuffer.address = graph_vram_allocate(ds.width, ds.height, psmz, GRAPH_ALIGN_PAGE);
	ds.zBuffer.enable = 0;
	ds.zBuffer.method = 0;
	ds.zBuffer.zsm = 0;
	ds.zBuffer.mask = 0;

	graph_set_mode(ds.gmode, ds.vmode, GRAPH_MODE_INTERLACED, GRAPH_DISABLE);
	graph_set_screen(0, 0, ds.width, ds.height);
	graph_set_bgcolor(0, 0, 0);
	graph_set_framebuffer_filtered(ds.frameBuffer.address, ds.width, psm, 0, 0);
	graph_enable_output();

	qword_t* q = DrawState::gCmdBuffer;
	std::memset(DrawState::gCmdBuffer, 0, DrawState::gCmdBufferSize);
	q = draw_setup_environment(q, 0, &ds.frameBuffer, &ds.zBuffer);
	//q = draw_primitive_xyoffset(q, 0, gOffsetX - (ds.width / 2), gOffsetY - (ds.height / 2));

	q = draw_finish(q);
	dma_channel_send_normal(DMA_CHANNEL_GIF, DrawState::gCmdBuffer, q - DrawState::gCmdBuffer, 0, 0);
	draw_wait_finish();

	return 0;
}