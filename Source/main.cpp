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
#define SCREEN_HEIGHT 480

//#define gftoi4(x) ((x) << 4)
#define gftoi4(x) ftoi4(x)

static qword_t* gBuf;
static constexpr int gBufSize = sizeof(qword_t) * 100;

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

    zbuffer_t z;
    z.address = graph_vram_allocate(width, height, psmz, GRAPH_ALIGN_PAGE);
    z.enable = 0;
    z.method = 0;
    z.zsm = 0;
    z.mask = 0;

    graph_set_mode(gmode, vmode, GRAPH_MODE_INTERLACED, GRAPH_DISABLE);
    graph_set_screen(OFFSET_X, OFFSET_Y, width, height);
    graph_set_bgcolor(0, 0, 0);
    graph_set_framebuffer_filtered(fb.address, width, psm, 0, 0);
    graph_enable_output();
    //graph_initialize(fb.address, width, height, psm, 0, 0);

    qword_t* q = gBuf;
    memset(gBuf, 0, gBufSize);
    q = draw_setup_environment(q, 0, &fb, &z);
    //q = draw_primitive_xyoffset(q, 0, 0, 0);
    //q = draw_disable_tests(q, 0, &z);
    //q = draw_clear(q, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);

    //q->dw[0]
    q = draw_finish(q);
    dma_channel_send_normal(DMA_CHANNEL_GIF, gBuf, q - gBuf, 0, 0);
    dma_wait_fast();

    //gs_finish();

    return 0;
}

static int tri[] = {
    320, 50, 0,
    600, 200, 1,
    20, 200, 0
};

#define SHIFT_AS_I64(x, b) (static_cast<int64_t>(x) << b)

qword_t* draw(qword_t* q){
    const uint64_t red = 0xf0;
    const uint64_t blue = 0x0f;
    const uint64_t green = 0x0f;

    //qword_t buf[80];
    //qword_t* q = buf;
    // SET PRIM
    q->dw[0] = 0x1000000000000001;
    q->dw[1] = 0x100000000000000e;
    q++;

    q->dw[0] = GS_SET_PRIM(GS_PRIM_TRIANGLE, 0, 0, 0, 0, 0, 0, 0, 0);
    q->dw[1] = GS_REG_PRIM;
    q++;

    // 6 registers, x1, EOP
    q->dw[0] = 0x7000000000000001;
    // GIFTag header - col, pos, col, pos, col, pos
    q->dw[1] = 0x0000000005151510;
    q++;

    q->dw[0] = GS_PRIM_TRIANGLE;
    q->dw[1] = 0;
    q++;

    for(int i = 0; i < 3; i++){
        q->dw[0] = (red & 0xff) | SHIFT_AS_I64((green & 0xff), 32);
        q->dw[1] = (blue & 0xff) | SHIFT_AS_I64(0x80, 32);
        q++;

        // 0xa -> 0xa0
        // fixed point format - xxxx xxxx xxxx . yyyy
        const int base = i * 3;
        const int x = gftoi4(tri[base + 0] + OFFSET_X);
        const int y = gftoi4(tri[base + 1] + OFFSET_Y);
        const int z = 0; // TODO

        q->dw[0] = x | SHIFT_AS_I64(y, 32);
        q->dw[1] = z;
        q++;
    }

    //print_buffer(buf, q - buf);
    dma_channel_send_normal(DMA_CHANNEL_GIF, gBuf, q - gBuf, 0, 0);
    dma_wait_fast();

    gs_finish();
    return q;
}

int main(){
    gBuf = static_cast<qword_t*>(malloc((100 * sizeof(qword_t))));

    printf("Hello World!\n");
    
    // Initialize DMA controller
    dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);
    
    // Initialize graphics mode
    gs_init(SCREEN_WIDTH, SCREEN_HEIGHT, GS_PSM_32, GS_PSMZ_24, GRAPH_MODE_NTSC, GRAPH_MODE_INTERLACED);

    //qword_t buf[20];
    graph_wait_vsync();
    while(true){
        dma_wait_fast();
        memset(gBuf, 0, gBufSize);
        qword_t* q = gBuf;

        // Clear the screen
        q = draw_clear(q, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 20, 20, 255);
        q = draw(q);
        q = draw_finish(q);
        dma_channel_send_normal(DMA_CHANNEL_GIF, gBuf, q - gBuf, 0, 0);
        //dma_wait_fast();
        //print_buffer(gBuf, q - gBuf);

        //dma_wait_fast();

        draw_wait_finish();
        graph_wait_vsync();

        //sleep(2);
    }
}