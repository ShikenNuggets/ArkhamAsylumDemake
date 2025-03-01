#include "FrameBuffer.hpp"

#include <draw.h>
#include <graph.h>
#include <gs_psm.h>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height){
	fb.width = width;
	fb.height = height;
	fb.mask = 0;
	fb.psm = GS_PSM_32;
	fb.address = graph_vram_allocate(fb.width, fb.height, fb.psm, GRAPH_ALIGN_PAGE);

	graph_initialize(fb.address, fb.width, fb.height, fb.psm, 0, 0);
}

FrameBuffer::~FrameBuffer(){
	graph_vram_free(fb.address);
}