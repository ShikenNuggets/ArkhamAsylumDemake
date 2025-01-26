#ifndef PS2E_DEPTH_BUFFER_H
#define PS2E_DEPTH_BUFFER_H

#include <draw_buffers.h>

class DepthBuffer{
public:
	DepthBuffer(unsigned int width, unsigned int height);
	~DepthBuffer();

	const zbuffer_t* Get() const{ return &zBuffer; }
	zbuffer_t* Get(){ return &zBuffer; }

private:
	zbuffer_t zBuffer;
};

#endif //!PS2E_DEPTH_BUFFER_H