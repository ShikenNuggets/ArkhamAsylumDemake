#pragma once

#include <cstddef>
#include <cstdint>

#include <libmpeg.h>
#include <packet.h>

class MoviePlayer3{
public:
	MoviePlayer3();
	~MoviePlayer3();

	void PlayVideo(const char* filePath, int width = 640, int height = 360);

private:
	static int SetDMACallback(void* userData);
	static void* InitCallback(void* userData, MPEGSequenceInfo* sequenceInfo);

	int SetDMA();
	void* InitCB(MPEGSequenceInfo* info);

	uint8_t* mpegData;
	uint8_t* transferPtr;
	size_t mpegDataSize;

	packet_t* transferPacket;
	packet_t* drawPacket;
	int textureAddress;
	uint8_t* decodedData;

	bool eof;
	int videoWidth;
	int videoHeight;
};
