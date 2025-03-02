#ifndef PS2E_RENDERER_HPP
#define PS2E_RENDERER_HPP

#include <cstdint>
#include <vector>

#include <packet.h>

#include "Camera.hpp"
#include "DepthBuffer.hpp"
#include "FrameBuffer.hpp"
#include "GameObject.hpp"

class Renderer{
public:
	Renderer(unsigned int width_, unsigned int height_);
	~Renderer();

	void Render(Camera& camera, const std::vector<GameObject*>& gameObjects);

	inline FrameBuffer& GetFrameBuffer(){ return frameBuffer; }
	inline const FrameBuffer& GetFrameBuffer() const{ return frameBuffer; }

	inline DepthBuffer& GetDepthBuffer(){ return depthBuffer; }
	inline const DepthBuffer& GetDepthBuffer() const{ return depthBuffer; }

	inline void SetClearColor(uint8_t r, uint8_t g, uint8_t b){
		clearR = r;
		clearG = g;
		clearB = b;
	}

	static constexpr inline int gOffsetX = 2048;
	static constexpr inline int gOffsetY = 2048;

private:
	FrameBuffer frameBuffer;
	DepthBuffer depthBuffer;

	unsigned int width;
	unsigned int height;

	packet_t* packets[2];
	int packetCtx;

	MATRIX viewScreenMatrix;

	uint8_t clearR;
	uint8_t clearG;
	uint8_t clearB;
};

#endif //!PS2E_RENDERER_HPP
