#pragma once

#include <cstddef>
#include <cstdint>
#include <ios>

#include <libmpeg.h>
#include <SDL3/SDL.h>

class MoviePlayer3{
public:
	MoviePlayer3(SDL_Renderer* inRenderer);
	~MoviePlayer3();

	void PlayVideo(const char* filePath, int width = 640, int height = 360);

private:
	static int SetDMACallback(void* userData);
	static void* InitCallback(void* userData, MPEGSequenceInfo* sequenceInfo);

	int SetDMA();
	void* InitCB(MPEGSequenceInfo* info);

	SDL_Renderer* renderer;
	SDL_Texture* videoTexture;

	uint8_t* mpegData;
	uint8_t* transferPtr;
	std::streamsize mpegDataSize;
	uint8_t* decodedData;

	bool eof;
	int videoWidth;
	int videoHeight;

	int mpegWidth = 0;
	int mpegHeight = 0;
};
