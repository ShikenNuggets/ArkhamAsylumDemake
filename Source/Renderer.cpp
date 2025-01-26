#include "Renderer.hpp"

#include <cstring>
#include <stdlib.h>

#include <dma.h>
#include <draw.h>
#include <draw_buffers.h>
#include <graph.h>
#include <graph_vram.h>
#include <packet.h>

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

Renderer::~Renderer() = default;