#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include <draw.h>
#include <draw2d.h>
#include <draw_types.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <dma.h>
#include <dma_tags.h>
#include <packet.h>

#include "DepthBuffer.h"
#include "FrameBuffer.h"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "Utils.hpp"

#include "mesh_data.h"

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

void* __gxx_personality_v0; //Need this to be defined somewhere due to exception handling being disabled

void mesh_transform(const Mesh& mesh, char* buffer, int cx, int cy, float scale, float tx, float ty)
{
	const int stride = mesh.vertexSize * 16;
	for(int i = 0; i < mesh.numVertex; i++){
		char* const vStart = buffer + (stride * i);
		float* pos = reinterpret_cast<float*>(vStart) + (mesh.vertexPosOffset * 16);
		float x = pos[0];
		float y = pos[1];
		float z = pos[2];
		//float w = pos[3];
		
		*((uint32_t*)pos) = ftoi4((x * scale) + tx) + cx;
		*((uint32_t*)(pos + 1)) = ftoi4((y * scale) + ty) + cy;
		*((uint32_t*)(pos + 2)) = static_cast<int>(z);
		pos[3] = 0.0f;
	}
}

// int main(){
// 	printf("[PS2E] Hello World!\n");
	
// 	constexpr size_t drawBufSize = sizeof(qword_t) * 20'000;
// 	qword_t* drawBuf = static_cast<qword_t*>(std::malloc(drawBufSize));

// 	// Initialize DMA controller
// 	dma_channel_initialize(DMA_CHANNEL_GIF, 0, 0);
// 	dma_channel_fast_waits(DMA_CHANNEL_GIF);

// 	// Initialize graphics mode
// 	DrawState ds{}; //TODO - Proper constructor/RAII
// 	ds.width = SCREEN_WIDTH;
// 	ds.height = SCREEN_HEIGHT;
// 	ds.vmode = graph_get_region();
// 	ds.gmode = GRAPH_MODE_INTERLACED;

// 	Renderer::gs_init(ds, GS_PSM_32, GS_PSMZ_24);

// 	Mesh m = Mesh(cubeModelFile);

// 	graph_wait_vsync();
// 	while(true){
// 		dma_wait_fast();
// 		qword_t* q = drawBuf;
// 		memset(drawBuf, 0, drawBufSize);

// 		//q = draw_disable_tests(q, 0, &ds.zBuffer);
// 		q = draw_clear(q, 0, static_cast<float>(Renderer::gOffsetX) - (SCREEN_WIDTH / 2.0f), static_cast<float>(Renderer::gOffsetY) - (SCREEN_HEIGHT / 2.0f), SCREEN_WIDTH, SCREEN_HEIGHT, 20, 20, 255);
// 		//q = draw_enable_tests(q, 0, &ds.zBuffer);
// 		//q = draw(q);

// 		static constexpr float cx = Renderer::gOffsetX + (SCREEN_WIDTH / 2.0f);
// 		static constexpr float cy = Renderer::gOffsetY + (SCREEN_HEIGHT / 2.0f);

// 		qword_t* modelVertsStart = q;

// 		std::memcpy(q, m.buffer, m.bufferLen);
// 		q += (m.bufferLen / 16);
// 		q = draw_finish(q);
// 		mesh_transform(m, reinterpret_cast<char*>(modelVertsStart + 4), ftoi4(cx), ftoi4(cy), 20.0f, 0.0f, 0.0f);

// 		dma_channel_send_normal(DMA_CHANNEL_GIF, drawBuf, q - drawBuf, 0, 0);
// 		draw_wait_finish();
// 		graph_wait_vsync();
// 	}
// }

VECTOR object_position = { 0.00f, 0.00f, 0.00f, 1.00f };
VECTOR object_rotation = { 0.00f, 0.00f, 0.00f, 1.00f };

VECTOR camera_position = { 0.00f, 0.00f, 100.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f,   0.00f, 1.00f };

void init_drawing_environment(FrameBuffer& frame, DepthBuffer& depth){
	packet_t *packet = packet_init(16,PACKET_NORMAL);

	// This is our generic qword pointer.
	qword_t *q = packet->data;

	// This will setup a default drawing environment.
	q = draw_setup_environment(q, 0, frame.Get(), depth.Get());

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	q = draw_primitive_xyoffset(q,0,(2048-320),(2048-240));

	// Finish setting up the environment.
	q = draw_finish(q);

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data,q - packet->data, 0, 0);
	dma_wait_fast();

	packet_free(packet);
}

int render(FrameBuffer& frame, DepthBuffer& depth){
	int context = 0;

	// Matrices to setup the 3D environment and camera
	MATRIX local_world;
	MATRIX world_view;
	MATRIX view_screen;
	MATRIX local_screen;

	VECTOR *temp_vertices;

	prim_t prim;
	color_t color;

	xyz_t   *verts;
	color_t *colors;

	// The data packets for double buffering dma sends.
	packet_t *packets[2];
	packet_t *current;
	qword_t *dmatag;

	packets[0] = packet_init(100,PACKET_NORMAL);
	packets[1] = packet_init(100,PACKET_NORMAL);

	// Allocate calculation space.
	temp_vertices = static_cast<VECTOR*>(memalign(128, sizeof(VECTOR) * vertex_count));

	// Allocate register space.
	verts  = static_cast<xyz_t*>(memalign(128, sizeof(vertex_t) * vertex_count));
	colors = static_cast<color_t*>(memalign(128, sizeof(color_t)  * vertex_count));

	// Define the triangle primitive we want to use.
	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_DISABLE;
	prim.fogging = DRAW_DISABLE;
	prim.blending = DRAW_DISABLE;
	prim.antialiasing = DRAW_ENABLE;
	prim.mapping_type = PRIM_MAP_ST;
	prim.colorfix = PRIM_UNFIXED;

	color.r = 0x80;
	color.g = 0x80;
	color.b = 0x80;
	color.a = 0x80;
	color.q = 1.0f;

	// Create the view_screen matrix.
	create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);

	// Wait for any previous dma transfers to finish before starting.
	dma_wait_fast();

	// The main loop...
	while(true){
		qword_t *q;

		current = packets[context];

		// Spin the cube a bit.
		object_rotation[0] += 0.008f; //while (object_rotation[0] > 3.14f) { object_rotation[0] -= 6.28f; }
		object_rotation[1] += 0.012f; //while (object_rotation[1] > 3.14f) { object_rotation[1] -= 6.28f; }

		// Create the local_world matrix.
		create_local_world(local_world, object_position, object_rotation);

		// Create the world_view matrix.
		create_world_view(world_view, camera_position, camera_rotation);

		// Create the local_screen matrix.
		create_local_screen(local_screen, local_world, world_view, view_screen);

		// Calculate the vertex values.
		calculate_vertices(temp_vertices, vertex_count, vertices, local_screen);

		// Convert floating point vertices to fixed point and translate to center of screen.
		draw_convert_xyz(verts, 2048, 2048, 32, vertex_count, (vertex_f_t*)temp_vertices);

		// Convert floating point colours to fixed point.
		draw_convert_rgbq(colors, vertex_count, (vertex_f_t*)temp_vertices, (color_f_t*)colours, 0x80);

		// Grab our dmatag pointer for the dma chain.
		dmatag = current->data;

		// Now grab our qword pointer and increment past the dmatag.
		q = dmatag;
		q++;

		// Clear framebuffer but don't update zbuffer.
		q = draw_disable_tests(q,0,depth.Get());
		q = draw_clear(q,0,2048.0f-320.0f,2048.0f-240.0f,640,480,0x00,0x00,0x00);
		q = draw_enable_tests(q,0,depth.Get());

		// Draw the triangles using triangle primitive type.
		q = draw_prim_start(q,0,&prim, &color);

		for(int i = 0; i < points_count; i++)
		{
			q->dw[0] = colors[points[i]].rgbaq;
			q->dw[1] = verts[points[i]].xyz;
			q++;
		}

		q = draw_prim_end(q,2,DRAW_RGBAQ_REGLIST);

		// Setup a finish event.
		q = draw_finish(q);

		// Define our dmatag for the dma chain.
		DMATAG_END(dmatag,(q-current->data)-1,0,0,0);

		// Now send our current dma chain.
		dma_wait_fast();
		dma_channel_send_chain(DMA_CHANNEL_GIF,current->data, q - current->data, 0, 0);

		// Now switch our packets so we can process data while the DMAC is working.
		context ^= 1;

		// Wait for scene to finish drawing
		draw_wait_finish();

		graph_wait_vsync();
	}

	packet_free(packets[0]);
	packet_free(packets[1]);

	return 0;
}

int main(){
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	FrameBuffer frame = FrameBuffer(640, 480);
	DepthBuffer depth = DepthBuffer(640, 480);

	init_drawing_environment(frame, depth);

	render(frame, depth);

	return 0;
}