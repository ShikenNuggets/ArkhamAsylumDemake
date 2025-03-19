#include "ScreenQuad.hpp"

#include <cstdlib>
#include <cstdio>

#include <draw.h>

#include "Debug.hpp"
#include "Renderer.hpp"

constexpr int quadVertexCount = 4;
constexpr int quadPointsCount = 6;

constexpr int quadPoints[quadPointsCount] = {
	0, 1, 2,
	1, 2, 3,
};

constexpr xyz_t quadVertexBuffer[quadVertexCount] = {
	-9223372034490199056ul,	// 37872, 36080
	-9223372034958846992ul,	// 37872, 28929
	-9223372034490209279ul,	// 27649, 36080
	-9223372034958857215ul	// 27649, 28929
};

constexpr color_t quadColorBuffer[quadVertexCount] = { 0ul, 0ul, 0ul, 0ul };

constexpr texel_t quadTexCoordBuffer[quadVertexCount] = {
	4575657222473777152ul,	// 1.0f, 1.0f
	1065353216ul,			// 1.0f, 0.0f
	4575657221408423936ul,	// 0.0f, 1.0f
	0ul						// 0.0f, 0.0f
};

ScreenQuad::ScreenQuad(){
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

qword_t* ScreenQuad::Render(qword_t* q){
	// Draw the triangles using triangle primitive type.
	// Use a 64-bit pointer to simplify adding data to the packet.
	u64* dw = reinterpret_cast<u64*>(draw_prim_start(q, 0, &prim, &color));

	for(int i = 0; i < quadPointsCount; i++){
		*dw = quadColorBuffer[quadPoints[i]].rgbaq; dw++;
		*dw = quadTexCoordBuffer[quadPoints[i]].uv; dw++;
		*dw = quadVertexBuffer[quadPoints[i]].xyz; dw++;
	}

	// Check if we're in middle of a qword or not.
	if(reinterpret_cast<u32>(dw) % 16){
		*dw = 0; dw++;
	}

	// Only 3 registers rgbaq/st/xyz were used (standard STQ reglist)
	return draw_prim_end(reinterpret_cast<qword_t*>(dw), 3, DRAW_STQ_REGLIST);
}
