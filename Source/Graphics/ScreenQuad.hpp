#pragma once

#include <draw_primitives.h>
#include <draw_types.h>
#include <math3d.h>

class ScreenQuad{
public:
	ScreenQuad();
	~ScreenQuad() = default;

	qword_t* Render(qword_t* q);

private:
	prim_t prim;
	color_t color;
};
