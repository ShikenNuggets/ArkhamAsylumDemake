#ifndef PS2E_RENDERER_HPP
#define PS2E_RENDERER_HPP

#include <draw_buffers.h>
#include <tamtypes.h>

#include "DepthBuffer.h"
#include "FrameBuffer.h"

class DrawState{
public:
	int width;
	int height;
	int vmode;
	int gmode;
	framebuffer_t frameBuffer;
	zbuffer_t zBuffer;

	static qword_t* gCmdBuffer;
	static constexpr inline int gCmdBufferSize = 40 * sizeof(qword_t);
};

class Renderer{
public:
	Renderer(unsigned int width_, unsigned int height_);
	~Renderer();

	static int gs_init(DrawState& ds, int psm, int psmz);

	FrameBuffer& GetFrameBuffer(){ return frameBuffer; }
	const FrameBuffer& GetFrameBuffer() const{ return frameBuffer; }

	DepthBuffer& GetDepthBuffer(){ return depthBuffer; }
	const DepthBuffer& GetDepthBuffer() const{ return depthBuffer; }

	static constexpr inline int gOffsetX = 2048;
	static constexpr inline int gOffsetY = 2048;

private:
	FrameBuffer frameBuffer;
	DepthBuffer depthBuffer;

	unsigned int width;
	unsigned int height;
};

#endif //!PS2E_RENDERER_HPP
