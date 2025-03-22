#pragma once

#include <cstdint>
#include <vector>

#include "TextureBuffer.hpp"

class MoviePlayer{
public:
	MoviePlayer(const char* moviePath, int16_t numFrames);
	~MoviePlayer();

	void Update();

private:
	std::vector<uint8_t> buffer;
	const char* path;
	TextureBuffer* currentFrame;
	int16_t currentFrameIdx;
	int16_t numFrames;

	void BindCurrentFrame();
};