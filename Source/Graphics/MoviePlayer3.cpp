#include "MoviePlayer3.hpp"

#include <algorithm>
#include <malloc.h>

#include <dma.h>
#include <kernel.h>

#include "Debug.hpp"
#include "Platform/DmaChannel.hpp"
#include "ThirdParty/stb/stb.h"

MoviePlayer3::MoviePlayer3(SDL_Renderer* inRenderer) : renderer(inRenderer), videoTexture(nullptr), decodedData(nullptr), eof(false), audioStream(nullptr)
{
}

MoviePlayer3::~MoviePlayer3()
{
	if (decodedData)
	{
		free(decodedData);
	}

	if (videoTexture)
	{
		SDL_DestroyTexture(videoTexture);
	}

	if (audioStream)
	{
		SDL_DestroyAudioStream(audioStream);
	}
}

void MoviePlayer3::PlayVideo(const char* filePath)
{
	auto eofPayload = std::vector<uint8_t>(4, 0x0);
	eofPayload[0] = 0x00;
	eofPayload[1] = 0x00;
	eofPayload[2] = 0x01;
	eofPayload[3] = 0xB7;

	videoFileBuffer = std::make_unique<FileBuffer>(filePath, videoBufferSize, eofPayload);

	// Audio Loading and Setup
	std::string audioFilePath = filePath;
	size_t lastDotPos = audioFilePath.find_last_of('.');
	if (lastDotPos != std::string::npos)
	{
		audioFilePath.replace(lastDotPos, audioFilePath.length() - lastDotPos, "_0.pcm");
	}
	else
	{
		audioFilePath += "_0.pcm";
	}

	audioFileBuffer = std::make_unique<FileBuffer>(audioFilePath, audioBufferSize);

	SDL_AudioSpec srcSpec;
	srcSpec.freq = 48000;
	srcSpec.format = SDL_AUDIO_S16LE;
	srcSpec.channels = 2;

	SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (audioDevice == 0)
	{
		LOG_ERROR("Failed to open physical audio device: %s", SDL_GetError());
		return;
	}
	
	audioStream = SDL_CreateAudioStream(&srcSpec, nullptr);
	if (!audioStream)
	{
		LOG_ERROR("Failed to create SDL audio stream: %s", SDL_GetError());
		return;
	}
	else
	{
		SDL_ResumeAudioStreamDevice(audioStream);
	}

	if (!SDL_BindAudioStream(audioDevice, audioStream))
	{
		LOG_ERROR("Failed to bind SDL audio stream: %s", SDL_GetError());
	}

	int bytesPerSecond = 48000 * 2 * sizeof(short);
	int targetQueueBytes = bytesPerSecond * 2;
	size_t totalAudioBytesSubmitted = 0;

	// Grab DMA channels in case they're not already initialized
	[[maybe_unused]] DmaChannel& GifChannel = DmaChannel::GIFChannel();
	[[maybe_unused]] DmaChannel& ToIPUChannel = DmaChannel::ToIPUChannel();

	s64 currentPresentationTimeStamp = 0;
	MPEG_Initialize(SetDMACallback, this, InitCallback, this, &currentPresentationTimeStamp);

	while (!eof)
	{
		// Audio Feeder
		if (audioStream && !audioFileBuffer->IsExhausted())
		{
			int queuedBytes = SDL_GetAudioStreamQueued(audioStream);
			if (queuedBytes >= 0 && queuedBytes < (targetQueueBytes / 4)) // Wait until the audio stream is actually running low
			{
				auto audioChunk = audioFileBuffer->GetChunk();
				if (!audioChunk.empty())
				{
					int spaceInQueue = targetQueueBytes - queuedBytes;
					int bytesToQueue = std::min<size_t>(audioChunk.size(), spaceInQueue);

					if (bytesToQueue > 0)
					{
						if (!SDL_PutAudioStreamData(audioStream, audioChunk.data(), bytesToQueue))
						{
							LOG_ERROR("Failed to push data to SDL audio stream: %s", SDL_GetError());
						}
						else
						{
							audioFileBuffer->Advance(bytesToQueue);
							totalAudioBytesSubmitted += bytesToQueue;
						}
					}
				}
			}
		}

		// Audio/Video Sync
		int audioQueueBytes = SDL_GetAudioStreamQueued(audioStream);
		int playedBytes = static_cast<int>(totalAudioBytesSubmitted) - audioQueueBytes;
        if (playedBytes < 0)
		{
			playedBytes = 0;
		}

        double audioTimeSeconds = static_cast<double>(playedBytes) / static_cast<double>(bytesPerSecond);
		double videoTimeSeconds = static_cast<double>(currentFrame) / fps;

		if (videoTimeSeconds > audioTimeSeconds + 0.01)
		{
			SDL_Delay(1); // Tiny sleep to slow down video and let audio catch up
			continue;
		}

		// Video Playback
		s64 framePresentationTimestamp{};
		int result = MPEG_Picture(decodedData, &framePresentationTimestamp);
		currentFrame++;
		if (eof || result == 0)
		{
			LOG_INFO("Reached end of video data");
			break;
		}

		if (result < 0 || result > 1)
		{
			LOG_ERROR("Error decoding video frame, MPEG_Picture returned: %d", result);
			break;
		}

		if (videoTimeSeconds < audioTimeSeconds - 0.033)
		{
			continue; // Drop the frame to speed up and stay in sync with audio
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
	if (eof)
	{
		return 0;
	}

	if (videoFileBuffer->IsExhausted())
	{
		eof = true;
		return 0;
	}

	std::span<uint8_t> chunk = videoFileBuffer->GetChunk();
	if (chunk.empty())
	{
		return 0;
	}

	const size_t bytesToSend = std::min<size_t>(chunk.size(), 2048);
	const size_t dmaTransferSize = (bytesToSend + 15) & ~15;

	DmaChannel& ToIPUChannel = DmaChannel::ToIPUChannel();
	ToIPUChannel.SendNormalBytes(chunk.data(), dmaTransferSize);
	videoFileBuffer->Advance(bytesToSend);
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
