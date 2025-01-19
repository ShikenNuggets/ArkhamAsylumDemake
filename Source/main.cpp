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

#include "Mesh.hpp"
#include "Renderer.hpp"
#include "Utils.hpp"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 448

static int tri[] = {
	320, 50, 0,
	600, 200, 1,
	20, 200, 0
};

qword_t* draw(qword_t* q){
	static constexpr uint64_t red = 0xf0;
	static constexpr uint64_t blue = 0x0f;
	static constexpr uint64_t green = 0x0f;
	
	// SET PRIM
	q->dw[0] = 0x1000000000000001;
	q->dw[1] = 0x000000000000000e;
	q++;

	q->dw[0] = GS_SET_PRIM(GS_PRIM_TRIANGLE, 0, 0, 0, 0, 0, 0, 0, 0);
	q->dw[1] = GS_REG_PRIM;
	q++;

	// 6 registers, x1, EOP
	q->dw[0] = 0x6000000000000001;
	// GIFTag header - col, pos, col, pos, col, pos
	q->dw[1] = 0x0000000000515151;
	q++;

	//static constexpr int cx = ftoi4(OFFSET_X - (SCREEN_WIDTH / 2));
	//akstatic constexpr int cy = ftoi4(OFFSET_Y - (SCREEN_HEIGHT / 2));

	for(int i = 0; i < 3; i++){
		q->dw[0] = (red & 0xff) | Utils::ShiftAsI64((green & 0xff), 32);
		q->dw[1] = (blue & 0xff) | Utils::ShiftAsI64(0x80, 32);
		q++;

		// 0xa -> 0xa0
		// fixed point format - xxxx xxxx xxxx . yyyy
		const int base = i * 3;
		const int x = ftoi4(tri[base + 0] + Renderer::gOffsetX); // + cx
		const int y = ftoi4(tri[base + 1] + Renderer::gOffsetY); // + cy
		const int z = 0; // TODO

		q->dw[0] = x | Utils::ShiftAsI64(y, 32);
		q->dw[1] = z;
		q++;
	}

	return q;
}

static constexpr const char* cubeModelFile = "host:cube.gif";

int main(){
	printf("[PS2E] Hello World!\n");
	
	constexpr size_t drawBufSize = sizeof(qword_t) * 20'000;
	qword_t* drawBuf = static_cast<qword_t*>(std::malloc(drawBufSize));

	// Initialize DMA controller
	dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	// Initialize graphics mode
	DrawState ds{}; //TODO - Proper constructor/RAII
	ds.width = SCREEN_WIDTH;
	ds.height = SCREEN_HEIGHT;
	ds.vmode = graph_get_region();
	ds.gmode = GRAPH_MODE_INTERLACED;

	Renderer::gs_init(ds, GS_PSM_32, GS_PSMZ_24);

	Mesh m = Mesh(cubeModelFile);

	graph_wait_vsync();
	while(true){
		dma_wait_fast();
		memset(drawBuf, 0, drawBufSize);
		qword_t* q = drawBuf;

		//q = draw_disable_tests(q, 0, &zBuffer);
		q = draw_clear(q, 0, static_cast<float>(Renderer::gOffsetX) - (SCREEN_WIDTH / 2.0f), static_cast<float>(Renderer::gOffsetY) - (SCREEN_HEIGHT / 2.0f), SCREEN_WIDTH, SCREEN_HEIGHT, 20, 20, 255);
		//q = draw_enable_tests(q, 0, &zBuffer);
		q = draw(q);
		//std::memcpy(q, m.buffer, m.bufferLen);
		//q += m.bufferLen / 16;

		q = draw_finish(q);
		dma_channel_send_normal(DMA_CHANNEL_GIF, drawBuf, q - drawBuf, 0, 0);
		draw_wait_finish();

		graph_wait_vsync();
		//sleep(2);
	}
}