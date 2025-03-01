#include "MeshComponent.hpp"

#include <cstdlib>
#include <cstdio>

#include <draw.h>

#include "Debug.hpp"
#include "Renderer.hpp"
#include "Assets/mesh_data.h"

MeshComponent::MeshComponent(){
	LOG_INFO("Allocating mesh buffers...");
	transformedVerts = static_cast<VECTOR*>(aligned_alloc(128, sizeof(VECTOR) * vertex_count));
	vertexBuffer  = static_cast<xyz_t*>(aligned_alloc(128, sizeof(xyz_t) * vertex_count));
	colorBuffer = static_cast<color_t*>(aligned_alloc(128, sizeof(color_t)  * vertex_count));
	texCoordBuffer = static_cast<texel_t*>(aligned_alloc(128, sizeof(texel_t) * vertex_count));
	LOG_INFO("Mesh buffers allocated");

	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_ENABLE;
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

MeshComponent::~MeshComponent(){
	free(transformedVerts);
	free(vertexBuffer);
	free(colorBuffer);
	free(texCoordBuffer);
}

void MeshComponent::Update(MATRIX& localScreen){
	calculate_vertices(transformedVerts, vertex_count, vertices, localScreen);

	// Convert floating point vertices to fixed point and translate to center of screen.
	draw_convert_xyz(vertexBuffer, Renderer::gOffsetX, Renderer::gOffsetY, 32, vertex_count, reinterpret_cast<vertex_f_t*>(transformedVerts));

	// Convert floating point colours to fixed point.
	draw_convert_rgbq(colorBuffer, vertex_count, reinterpret_cast<vertex_f_t*>(transformedVerts), reinterpret_cast<color_f_t*>(colours), color.a);

	// Generate the ST register values.
	draw_convert_st(texCoordBuffer, vertex_count, reinterpret_cast<vertex_f_t*>(transformedVerts), reinterpret_cast<texel_f_t*>(coordinates));
}

qword_t* MeshComponent::Render(qword_t* q){
	// Draw the triangles using triangle primitive type.
	// Use a 64-bit pointer to simplify adding data to the packet.
	u64* dw = reinterpret_cast<u64*>(draw_prim_start(q, 0, &prim, &color));

	for(int i = 0; i < points_count; i++){
		*dw = colorBuffer[points[i]].rgbaq; dw++;
		*dw = texCoordBuffer[points[i]].uv; dw++;
		*dw = vertexBuffer[points[i]].xyz; dw++;
	}

	// Check if we're in middle of a qword or not.
	if(reinterpret_cast<u32>(dw) % 16){
		*dw = 0; dw++;
	}

	// Only 3 registers rgbaq/st/xyz were used (standard STQ reglist)
	return draw_prim_end(reinterpret_cast<qword_t*>(dw), 3, DRAW_STQ_REGLIST);
}