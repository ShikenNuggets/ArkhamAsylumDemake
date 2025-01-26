#ifndef PS2E_FRAME_BUFFER_H
#define PS2E_FRAME_BUFFER_H

#include <draw_buffers.h>

class FrameBuffer{
public:
	FrameBuffer(unsigned int width, unsigned int height);
	~FrameBuffer();

	const framebuffer_t* Get() const{ return &fb; }
	framebuffer_t* Get(){ return &fb; }

private:
	framebuffer_t fb;
};


#endif //!PS2E_FRAME_BUFFER_H