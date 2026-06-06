#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <memory>

#include <libmpeg.h>
#include <SDL3/SDL.h>

#include "Utils/FileBuffer.hpp"

class MoviePlayer3
{
public:
	MoviePlayer3(SDL_Renderer* inRenderer);
	~MoviePlayer3();

	void PlayVideo(const char* filePath);

private:
	std::unique_ptr<FileBuffer> videoFileBuffer;

	static int SetDMACallback(void* userData);
	static void* InitCallback(void* userData, MPEGSequenceInfo* sequenceInfo);

	int SetDMA();
	void* InitCB(MPEGSequenceInfo* info);

	SDL_Renderer* renderer;
	SDL_Texture* videoTexture;

	static constexpr size_t videoBufferSize = 512 * 1024; // 512 KB per buffer

	uint8_t* mpegBuffers[2];
	size_t bufferSizes[2];
	std::atomic<bool> bufferReady[2];

	uint8_t* decodedData;
	bool eof;

	int mpegWidth = 0;
	int mpegHeight = 0;

	size_t currentFrame = 0;
	double fps = 29.97;

	SDL_AudioStream* audioStream;
};
