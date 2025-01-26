#include "MeshComponent.hpp"

#include <cstdlib>
#include <cstdio>

#include <draw.h>

#include "Renderer.hpp"
#include "mesh_data.h"

MeshComponent::MeshComponent(){
	std::printf("Allocating buffers...\n");
	transformedVerts = static_cast<VECTOR*>(aligned_alloc(128, sizeof(VECTOR) * vertex_count));
	vertexBuffer  = static_cast<xyz_t*>(aligned_alloc(128, sizeof(vertex_t) * vertex_count));
	colorBuffer = static_cast<color_t*>(aligned_alloc(128, sizeof(color_t)  * vertex_count));
	std::printf("Buffers allocated\n");

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
}

MeshComponent::~MeshComponent(){}

void MeshComponent::Update(MATRIX& localScreen){
	calculate_vertices(transformedVerts, vertex_count, vertices, localScreen);

	// Convert floating point vertices to fixed point and translate to center of screen.
	draw_convert_xyz(vertexBuffer, Renderer::gOffsetX, Renderer::gOffsetY, 32, vertex_count, (vertex_f_t*)transformedVerts);

	// Convert floating point colours to fixed point.
	draw_convert_rgbq(colorBuffer, vertex_count, (vertex_f_t*)transformedVerts, (color_f_t*)colours, 0x80);
}

qword_t* MeshComponent::Render(qword_t* q){
	q = draw_prim_start(q, 0, &prim, &color);

	for(int i = 0; i < points_count; i++){
		q->dw[0] = colorBuffer[points[i]].rgbaq;
		q->dw[1] = vertexBuffer[points[i]].xyz;
		q++;
	}

	return draw_prim_end(q, 2, DRAW_RGBAQ_REGLIST);
}