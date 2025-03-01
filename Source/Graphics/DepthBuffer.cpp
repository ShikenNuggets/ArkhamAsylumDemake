#include "DepthBuffer.hpp"

#include <draw.h>
#include <gs_psm.h>
#include <graph_vram.h>

DepthBuffer::DepthBuffer(unsigned int width, unsigned int height){
	zBuffer.enable = DRAW_ENABLE;
	zBuffer.mask = 0;
	zBuffer.method = ZTEST_METHOD_GREATER_EQUAL;
	zBuffer.zsm = GS_ZBUF_32;
	zBuffer.address = graph_vram_allocate(width, height, zBuffer.zsm, GRAPH_ALIGN_PAGE);
}

DepthBuffer::~DepthBuffer(){
	graph_vram_free(zBuffer.address);
}