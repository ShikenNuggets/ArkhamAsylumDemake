#include "TextureBuffer.hpp"

#include <dma.h>
#include <draw.h>
#include <gs_psm.h>
#include <graph_vram.h>
#include <packet.h>

TextureBuffer::TextureBuffer(unsigned int width_, unsigned int height_, uint8_t* data) : width(0), height(0){
	texBuffer.address = 0;
	LoadNewTexture(width_, height_, data);
	Bind();
}

TextureBuffer::~TextureBuffer(){
	graph_vram_free(texBuffer.address);
}

void TextureBuffer::LoadNewTexture(unsigned int width_, unsigned int height_, uint8_t* data){
	AllocateVRAM(width_, height_);
	if(texBuffer.address == 0){
		return;
	}

	packet_t* packet = packet_init(50, PACKET_NORMAL);
	
	qword_t* q = packet->data;
	q = draw_texture_transfer(q, data, width, height, texBuffer.psm, texBuffer.address, texBuffer.width);
	q = draw_texture_flush(q);
	
	dma_channel_send_chain(DMA_CHANNEL_GIF, packet->data, q - packet->data, 0, 0);
	dma_wait_fast();

	packet_free(packet);
}

void TextureBuffer::Bind(){
	// Prepare the texture for use
	packet_t* packet = packet_init(10, PACKET_NORMAL); // TODO - Should I just reuse the last packet?
	qword_t* q = packet->data;

	lod_t lod;
	lod.calculation = LOD_USE_K;
	lod.max_level = 0;
	lod.mag_filter = LOD_MAG_NEAREST;
	lod.min_filter = LOD_MIN_NEAREST;
	lod.l = 0;
	lod.k = 0;

	texBuffer.info.width = draw_log2(width);
	texBuffer.info.height = draw_log2(height);
	texBuffer.info.components = TEXTURE_COMPONENTS_RGB;
	texBuffer.info.function = TEXTURE_FUNCTION_DECAL;
	
	clutbuffer_t clut;
	clut.storage_mode = CLUT_STORAGE_MODE1;
	clut.start = 0;
	clut.psm = 0;
	clut.load_method = CLUT_NO_LOAD;
	clut.address = 0;

	q = draw_texture_sampling(q, 0, &lod);
	q = draw_texturebuffer(q, 0, &texBuffer, &clut);

	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data,q - packet->data, 0, 0);
	dma_wait_fast();

	packet_free(packet);
}

void TextureBuffer::AllocateVRAM(unsigned int width_, unsigned int height_){
	if(width_ == width && height == height_){
		return;
	}

	if(texBuffer.address != 0){
		graph_vram_free(texBuffer.address);
	}

	width = width_;
	height = height_;
	if(width == 0 || height == 0){
		return;
	}

	texBuffer.width = width_;
	texBuffer.psm = GS_PSM_24;
	texBuffer.address = graph_vram_allocate(width_, height_, GS_PSM_24, GRAPH_ALIGN_BLOCK);
}
