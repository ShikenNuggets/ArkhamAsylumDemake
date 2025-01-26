#ifndef PS2E_MESH_COMPONENT_HPP
#define PS2E_MESH_COMPONENT_HPP

#include <draw_primitives.h>
#include <draw_types.h>
#include <math3d.h>

class MeshComponent{
public:
	MeshComponent();
	~MeshComponent();

	void Update(MATRIX& localScreen);
	qword_t* Render(qword_t* q);

private:
	VECTOR* transformedVerts;
	xyz_t* vertexBuffer;
	color_t* colorBuffer;

	prim_t prim;
	color_t color;
};

#endif //!PS2E_MESH_COMPONENT_HPP