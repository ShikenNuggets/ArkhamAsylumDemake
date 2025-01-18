#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <dma.h>
#include <dma_tags.h>

#define OFFSET_X 0
#define OFFSET_Y 0

int print_buffer(qword_t* b, int len){
    printf("-- buffer\n");
    for(int i = 0; i < len; i++){
        printf("%016llx %016llx\n", b->dw[0], b->dw[1]);
        b++;
    }
    printf("-- /buffer\n");
    return 0;
}

int gs_finish(){
    qword_t buf[50];
    qword_t* q = buf;
    // TODO - Cleanup
    q = draw_primitive_xyoffset(q, 0, 0, 0);
    q = draw_finish(q);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q - buf, 0, 0);
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
    z.zsm = psmz;
    z.mask = 0;

    //unsigned int zbuf_address = graph_vram_allocate(w, h, psmz, GRAPH_ALIGN_PAGE);
    graph_set_mode(gmode, vmode, GRAPH_MODE_INTERLACED, GRAPH_ENABLE);
    graph_set_screen(OFFSET_X, OFFSET_Y, width, height);
    graph_set_bgcolor(0, 0, 0);
    graph_set_framebuffer_filtered(fb.address, width, psm, 0, 0);
    graph_enable_output();

    qword_t buf[100];
    qword_t* q = buf;
    q = draw_setup_environment(q, 0, &fb, &z);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q - buf, 0, 0);
    dma_wait_fast();

    gs_finish();

    return 0;
}

static int tri[] = {
    320, 50, 0,
    600, 200, 0,
    20, 200, 0
};

#define SHIFT_AS_I64(x, b) (static_cast<int64_t>(x) << b)

int draw(){
    uint64_t red = 0xf0;
    uint64_t blue = 0x0f;
    uint64_t green = 0x0f;

    qword_t buf[50];
    qword_t* q = buf;
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
        q->dw[0] = (tri[base + 0] << 4) | SHIFT_AS_I64((tri[base + 1] << 4), 32);
        q->dw[1] = (tri[base + 2] << 4);
        q++;
    }

    print_buffer(buf, q - buf);
    dma_channel_send_normal(DMA_CHANNEL_GIF, buf, q - buf, 0, 0);
    dma_wait_fast();

    gs_finish();
    return 0;
}

int main(){
    printf("Hello World!\n");
    
    // Initialize DMA controller
    dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);
    
    // Initialize graphics mode
    gs_init(640, 480, GS_PSM_32, GS_PSMZ_24, GRAPH_MODE_NTSC, GRAPH_MODE_INTERLACED);

    // Clear the screen
    draw();

    // Build buffer with triangle data

    while(1){ continue; }
}