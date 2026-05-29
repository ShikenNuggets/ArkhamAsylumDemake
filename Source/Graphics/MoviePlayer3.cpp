#include "MoviePlayer3.hpp"

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <dma.h>
#include <dma_tags.h>
#include <draw.h>
#include <gif_tags.h>
#include <graph.h>
#include <gs_psm.h>
#include <gs_gp.h>
#include <kernel.h>
#include <libmpeg.h>
#include <packet.h>

#include "Debug.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Platform/DmaChannel.hpp"

/* get the whole file (or first 24MB) into memory for simplicity */
#define MAX_SIZE (1024 * 1024 * 24)

MoviePlayer3::MoviePlayer3() : mpegData(nullptr), transferPtr(nullptr), mpegDataSize(0), transferPacket(nullptr), drawPacket(nullptr), textureAddress(0), decodedData(nullptr), eof(false)
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

	if (transferPacket)
	{
		packet_free(transferPacket);
	}

	if (drawPacket)
	{
		packet_free(drawPacket);
	}
}

void MoviePlayer3::PlayVideo(const char* filePath, int width, int height)
{
	int screenWidth = 640;
	int screenHeight = 448;

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
	DmaChannel GifChannel = DmaChannel::GIFChannel();
	DmaChannel ToIPUChannel = DmaChannel::ToIPUChannel();

	auto frameBuffer = FrameBuffer(screenWidth, screenHeight);

	zbuffer_t z{};
	z.enable = 0;

	packet_t* envPacket = packet_init(10, PACKET_NORMAL);
	qword_t* q = envPacket->data;
	q = draw_setup_environment(q, 0, frameBuffer.Get(), &z);
	q = draw_clear(q, 0, 0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0, 0, 0);
	q = draw_finish(q);
	dma_channel_send_normal(DMA_CHANNEL_GIF, envPacket->data, q - envPacket->data, 0, 0);
	packet_free(envPacket);

	s64 currentPresentationTimeStamp = 0;
	MPEG_Initialize(SetDMACallback, this, InitCallback, this, &currentPresentationTimeStamp);

	while (!eof)
	{
		s64 framePresentationTimestamp{};
		int result = MPEG_Picture(decodedData, &framePresentationTimestamp);
		if (eof)
		{
			LOG_INFO("Reached end of video data");
			break;
		}

		if (result != 1)
		{
			LOG_ERROR("Error decoding video frame, MPEG_Picture returned: %d", result);
			return;
		}

		dma_wait_fast();
        
        dma_channel_send_chain(DMA_CHANNEL_GIF, transferPacket->data, transferPacket->qwc, 0, 0);
        dma_wait_fast();

        dma_channel_send_normal(DMA_CHANNEL_GIF, drawPacket->data, drawPacket->qwc, 0, 0);
        
        graph_wait_vsync();
        graph_wait_vsync();
	}

	MPEG_Destroy();
}

int MoviePlayer3::SetDMACallback(void* userData)
{
	return static_cast<MoviePlayer3*>(userData)->SetDMA();	
}

int MoviePlayer3::SetDMA()
{
	if (transferPtr - mpegData >= mpegDataSize)
	{
		LOG_INFO("EOF");
		eof = true; // Mark EOF if we've sent all the data
		return 0;
	}

	dma_channel_wait(DMA_CHANNEL_toIPU, 0);
	dma_channel_send_normal(DMA_CHANNEL_toIPU, transferPtr, 2048 >> 4, 0, 0);
	transferPtr += 2048;

	return 1;	
}

void* MoviePlayer3::InitCallback(void* userData, MPEGSequenceInfo* sequenceInfo)
{
	return static_cast<MoviePlayer3*>(userData)->InitCB(sequenceInfo);
}

void* MoviePlayer3::InitCB(MPEGSequenceInfo* info)
{
	int lDataSize = info->m_Width * info->m_Height * 4;
    if (!decodedData)
	{
		decodedData = static_cast<uint8_t*>(memalign(64, lDataSize));
	}

    SyncDCache(decodedData, decodedData + lDataSize);

	if (textureAddress == 0)
	{
		textureAddress = graph_vram_allocate(info->m_Width, info->m_Height, GS_PSM_32, GRAPH_ALIGN_PAGE) >> 5;
		LOG_INFO("Allocated off-screen texture %dx%d\n", info->m_Width, info->m_Height);
	}

    int lMBW = info->m_Width >> 4;
    int lMBH = info->m_Height >> 4;
    int lTBW = (info->m_Width + 63) >> 6;
    
    // CRITICAL: Aim the transfer at the off-screen texture, NOT 0!
    int blockAddr = textureAddress; 

    // -------------------------------------------------------------
    // 1. TRANSFER PACKET (RAM -> Off-screen VRAM)
    // -------------------------------------------------------------
    if (transferPacket)
	{
		packet_free(transferPacket);
	}

    transferPacket = packet_init((12 + 12 * lMBW * lMBH) >> 1, PACKET_NORMAL);
    qword_t* q = transferPacket->data;

    DMATAG_CNT(q, 4, 0, 0, 0); q++;
    PACK_GIFTAG(q, GIF_SET_TAG(3, 0, 0, 0, 0, 1), GIF_REG_AD); q++;
    PACK_GIFTAG(q, GS_SET_SCISSOR(0, 639, 0, 447), GS_REG_SCISSOR_1); q++;
    PACK_GIFTAG(q, GS_SET_TRXREG(16, 16), GS_REG_TRXREG); q++;
    
    // Uploading to blockAddr (m_texAddr)
    PACK_GIFTAG(q, GS_SET_BITBLTBUF(0, 0, 0, blockAddr, lTBW, GS_PSM_32), GS_REG_BITBLTBUF); q++;

    uint8_t* lpImg = decodedData;
    for (int lY = 0; lY < info->m_Height; lY += 16)
	{
        for (int lX = 0; lX < info->m_Width; lX += 16, lpImg += 1024)
		{
            DMATAG_CNT(q, 4, 0, 0, 0); q++;
            PACK_GIFTAG(q, GIF_SET_TAG(2, 0, 0, 0, 0, 1), GIF_REG_AD); q++;
            // Write perfectly to 0,0 of the texture buffer
            PACK_GIFTAG(q, GS_SET_TRXPOS(0, 0, lX, lY, 0), GS_REG_TRXPOS); q++;
            PACK_GIFTAG(q, GS_SET_TRXDIR(0), GS_REG_TRXDIR); q++;
            PACK_GIFTAG(q, GIF_SET_TAG(64, 1, 0, 0, 2, 0), 0); q++;
            DMATAG_REF(q, 64, reinterpret_cast<unsigned int>(lpImg), 0, 0, 0); q++;
        }
    }

    transferPacket->qwc = q - transferPacket->data;
    SyncDCache(transferPacket->data, (uint8_t*)transferPacket->data + (transferPacket->qwc * 16));

    // -------------------------------------------------------------
    // 2. DRAW PACKET (Off-screen VRAM -> Centered on TV)
    // -------------------------------------------------------------
    if (drawPacket)
	{
		packet_free(drawPacket);
	}

    drawPacket = packet_init(8, PACKET_NORMAL); 
    q = drawPacket->data;

	int lTW = 10; // 2^10 = 1024 (Safely contains 640)
    int lTH = 9;  // 2^9 = 512  (Safely contains 368)
    
    PACK_GIFTAG(q, GIF_SET_TAG(7, 1, 0, 0, 0, 1), GIF_REG_AD); q++;
    PACK_GIFTAG(q, GS_SET_TEX1(0, 0, 0, 0, 0, 0, 0), GS_REG_TEX1_1); q++;
    PACK_GIFTAG(q, GS_SET_TEX0(blockAddr, lTBW, GS_PSM_32, lTW, lTH, 1, 1, 0, 0, 0, 0, 0), GS_REG_TEX0_1); q++;
    PACK_GIFTAG(q, GS_SET_PRIM(6, 0, 1, 0, 0, 0, 1, 0, 0), GS_REG_PRIM); q++;
    PACK_GIFTAG(q, GS_SET_UV(0, 0), GS_REG_UV); q++;
    PACK_GIFTAG(q, GS_SET_XYZ((2048 + 0) << 4, (2048 + 44) << 4, 0), GS_REG_XYZ2); q++;
    PACK_GIFTAG(q, GS_SET_UV(640 << 4, 360 << 4), GS_REG_UV); q++;
    PACK_GIFTAG(q, GS_SET_XYZ((2048 + 640) << 4, (2048 + 404) << 4, 0), GS_REG_XYZ2); q++;\

    drawPacket->qwc = q - drawPacket->data;
    SyncDCache(drawPacket->data, (uint8_t*)drawPacket->data + (drawPacket->qwc * 16));

    return decodedData;
}
