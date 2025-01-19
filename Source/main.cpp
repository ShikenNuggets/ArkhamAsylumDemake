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

#define OFFSET_X 2048
#define OFFSET_Y 2048

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 448

static qword_t* gBuf;
static constexpr int gBufSize = sizeof(qword_t) * 100;

static zbuffer_t zBuffer;

int print_buffer(qword_t* b, int len){
	printf("-- buffer\n");
	for(int i = 0; i < len; i++){
		printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
		b++;
	}
	printf("-- /buffer\n");
	return 0;
}

// TODO: Maybe FINISH needs to be in the same GS packet
int gs_finish(){
	qword_t* q = gBuf;
	q = draw_finish(q);
	dma_channel_send_normal(DMA_CHANNEL_GIF, gBuf, q - gBuf, 0, 0);
	dma_wait_fast();
	return 0;
}

int gs_init(int width, int height, int psm, int psmz, int vmode, int gmode){
	framebuffer_t fb{};
	fb.address = graph_vram_allocate(width, height, psm, GRAPH_ALIGN_PAGE);
	fb.width = width;
	fb.height = height;
	fb.psm = psm;
	fb.mask = 0;

	zBuffer.address = graph_vram_allocate(width, height, psmz, GRAPH_ALIGN_PAGE);
	zBuffer.enable = 0;
	zBuffer.method = 0;
	zBuffer.zsm = 0;
	zBuffer.mask = 0;

	graph_set_mode(gmode, vmode, GRAPH_MODE_INTERLACED, GRAPH_DISABLE);
	graph_set_screen(0, 0, width, height);
	graph_set_bgcolor(0, 0, 0);
	graph_set_framebuffer_filtered(fb.address, width, psm, 0, 0);
	graph_enable_output();

	qword_t* q = gBuf;
	memset(gBuf, 0, gBufSize);
	q = draw_setup_environment(q, 0, &fb, &zBuffer);
	//q = draw_primitive_xyoffset(q, 0, OFFSET_X - (SCREEN_WIDTH / 2), OFFSET_Y - (SCREEN_HEIGHT / 2));

	q = draw_finish(q);
	dma_channel_send_normal(DMA_CHANNEL_GIF, gBuf, q - gBuf, 0, 0);
	draw_wait_finish();

	return 0;
}

static int tri[] = {
	320, 50, 0,
	600, 200, 1,
	20, 200, 0
};

#define SHIFT_AS_I64(x, b) (static_cast<int64_t>(x) << b)

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
		q->dw[0] = (red & 0xff) | SHIFT_AS_I64((green & 0xff), 32);
		q->dw[1] = (blue & 0xff) | SHIFT_AS_I64(0x80, 32);
		q++;

		// 0xa -> 0xa0
		// fixed point format - xxxx xxxx xxxx . yyyy
		const int base = i * 3;
		const int x = ftoi4(tri[base + 0] + OFFSET_X); // + cx
		const int y = ftoi4(tri[base + 1] + OFFSET_Y); // + cy
		const int z = 0; // TODO

		q->dw[0] = x | SHIFT_AS_I64(y, 32);
		q->dw[1] = z;
		q++;
	}

	return q;
}

int main(){
	printf("Hello World!\n");
	
	gBuf = static_cast<qword_t*>(std::malloc(gBufSize));

	// Initialize DMA controller
	dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	// Initialize graphics mode
	gs_init(SCREEN_WIDTH, SCREEN_HEIGHT, GS_PSM_32, GS_PSMZ_24, graph_get_region(), GRAPH_MODE_INTERLACED);

	graph_wait_vsync();
	while(true){
		dma_wait_fast();
		memset(gBuf, 0, gBufSize);
		qword_t* q = gBuf;

		//q = draw_disable_tests(q, 0, &zBuffer);
		q = draw_clear(q, 0, static_cast<float>(OFFSET_X) - (SCREEN_WIDTH / 2.0f), static_cast<float>(OFFSET_Y) - (SCREEN_HEIGHT / 2.0f), SCREEN_WIDTH, SCREEN_HEIGHT, 20, 20, 255);
		//q = draw_enable_tests(q, 0, &zBuffer);
		q = draw(q);
		q = draw_finish(q);
		dma_channel_send_normal(DMA_CHANNEL_GIF, gBuf, q - gBuf, 0, 0);
		draw_wait_finish();

		graph_wait_vsync();
		//sleep(2);
	}
}