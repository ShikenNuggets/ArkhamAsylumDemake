#include "MoviePlayer3.hpp"

#include <fstream>
#include <malloc.h>

#include <dma.h>
#include <kernel.h>

#include "Debug.hpp"
#include "Platform/DmaChannel.hpp"

/* get the whole file (or first 24MB) into memory for simplicity */
#define MAX_SIZE (1024 * 1024 * 24)

MoviePlayer3::MoviePlayer3(SDL_Renderer* inRenderer) : renderer(inRenderer), videoTexture(nullptr), mpegData(nullptr), transferPtr(nullptr), mpegDataSize(0), decodedData(nullptr), eof(false)
{
}

MoviePlayer3::~MoviePlayer3()
{
	if (mpegData)
	{
		free(mpegData);
	}

	if (decodedData)
	{
		free(decodedData);
	}

	if (videoTexture)
	{
		SDL_DestroyTexture(videoTexture);
	}
}

void MoviePlayer3::PlayVideo(const char* filePath, int width, int height)
{
	videoWidth = width;
	videoHeight = height;

	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		LOG_ERROR("Could not open file: %s", filePath);
		return;
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	if (size <= 0)
	{
		LOG_ERROR("Could not obtain file size for: %s", filePath);
		return;
	}

	mpegDataSize = (size > MAX_SIZE) ? MAX_SIZE : size;
	mpegData = static_cast<uint8_t*>(memalign(64, mpegDataSize + 4));
	if (!mpegData)
	{
		LOG_ERROR("Could not allocate memory for MPEG data");
		return;
	}

	if (!file.read(reinterpret_cast<char*>(mpegData), mpegDataSize))
	{
		LOG_ERROR("Could not read file: %s", filePath);
		return;
	}

	// Inject end code so playback always stops gracefully even if the end code is missing or video is corrupted/malformed
	mpegData[mpegDataSize]     = 0x00;
    mpegData[mpegDataSize + 1] = 0x00;
    mpegData[mpegDataSize + 2] = 0x01;
    mpegData[mpegDataSize + 3] = 0xB7;
    mpegDataSize += 4;

	transferPtr = mpegData;
	eof = false;

	// Grab DMA channels in case they're not already initialized
	[[maybe_unused]] DmaChannel& GifChannel = DmaChannel::GIFChannel();
	[[maybe_unused]] DmaChannel& ToIPUChannel = DmaChannel::ToIPUChannel();

	s64 currentPresentationTimeStamp = 0;
	MPEG_Initialize(SetDMACallback, this, InitCallback, this, &currentPresentationTimeStamp);

	while (!eof)
	{
		s64 framePresentationTimestamp{};
		int result = MPEG_Picture(decodedData, &framePresentationTimestamp);
		if (eof || result == 0)
		{
			LOG_INFO("Reached end of video data");
			break;
		}

		if (result < 0 || result > 1)
		{
			LOG_ERROR("Error decoding video frame, MPEG_Picture returned: %d", result);
			return;
		}

		dma_wait_fast();

		uint8_t* linearPixels = nullptr;
		int pitch = 0;
		if (SDL_LockTexture(videoTexture, nullptr, reinterpret_cast<void**>(&linearPixels), &pitch))
		{
			uint8_t* lpImg = decodedData;
			for (int lY = 0; lY < mpegHeight; lY += 16)
			{
				for (int lX = 0; lX < mpegWidth; lX += 16)
				{
					for (int row = 0; row < 16; ++row)
					{
						uint8_t* dest = linearPixels + ((lY + row + 1) * pitch) + ((lX + 1) * 4);
						memcpy(dest, lpImg, 16 * 4);
						lpImg += 16 * 4;
					}
				}
			}

			SDL_UnlockTexture(videoTexture);
		}
		else
		{
			LOG_ERROR("Failed to lock SDL texture for drawing: %s", SDL_GetError());
			return;
		}

		SDL_FRect srcRect = { 
            1.0f, 
            1.0f, 
            static_cast<float>(mpegWidth), 
            static_cast<float>(mpegHeight) 
        };

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, videoTexture, &srcRect, nullptr);
		SDL_RenderPresent(renderer);
	}

	MPEG_Destroy();
}

int MoviePlayer3::SetDMACallback(void* userData)
{
	return static_cast<MoviePlayer3*>(userData)->SetDMA();	
}

int MoviePlayer3::SetDMA()
{
	DmaChannel& ToIPUChannel = DmaChannel::ToIPUChannel();

	if (transferPtr - mpegData >= mpegDataSize)
	{
		eof = true; // Mark EOF if we've sent all the data
		return 0;
	}

	ToIPUChannel.Wait();
	ToIPUChannel.SendNormalBytes(transferPtr, 2048);
	transferPtr += 2048;

	return 1;	
}

void* MoviePlayer3::InitCallback(void* userData, MPEGSequenceInfo* sequenceInfo)
{
	return static_cast<MoviePlayer3*>(userData)->InitCB(sequenceInfo);
}

void* MoviePlayer3::InitCB(MPEGSequenceInfo* info)
{
	mpegWidth = info->m_Width;
	mpegHeight = info->m_Height;

	int lDataSize = mpegWidth * mpegHeight * 4;
    if (!decodedData)
	{
		decodedData = static_cast<uint8_t*>(memalign(64, lDataSize));
	}

    SyncDCache(decodedData, decodedData + lDataSize);

	if (videoTexture == nullptr)
	{
		videoTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, mpegWidth + 2, mpegHeight + 2);

		SDL_SetTextureBlendMode(videoTexture, SDL_BLENDMODE_NONE);
		SDL_SetTextureScaleMode(videoTexture, SDL_SCALEMODE_LINEAR);

		void* pixels = nullptr;
		int pitch = 0;
		if (SDL_LockTexture(videoTexture, nullptr, &pixels, &pitch))
		{
			memset(pixels, 0, pitch * mpegHeight);
			SDL_UnlockTexture(videoTexture);
		}
		else
		{
			LOG_ERROR("Failed to lock SDL texture for clearing: %s", SDL_GetError());
		}

		LOG_INFO("Allocated SDL Streaming Texture %dx%d", mpegWidth, mpegHeight);
	}

    return decodedData;
}
