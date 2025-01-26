#include "FrameBuffer.h"

#include <draw.h>
#include <graph.h>
#include <gs_psm.h>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height){
	fb.address = graph_vram_allocate(width, height, GS_PSM_32, GRAPH_ALIGN_PAGE);
	fb.width = 640;
	fb.height = 480;
	fb.mask = 0;
	fb.psm = GS_PSM_32;

	graph_initialize(fb.address, width, height, GS_PSM_32, 0, 0);
}

FrameBuffer::~FrameBuffer(){
	graph_vram_free(fb.address);
}