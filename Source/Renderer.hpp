#ifndef PS2E_RENDERER_HPP
#define PS2E_RENDERER_HPP

#include "DepthBuffer.hpp"
#include "FrameBuffer.hpp"

class Renderer{
public:
	Renderer(unsigned int width_, unsigned int height_);
	~Renderer();

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
