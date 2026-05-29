#include "DmaChannel.hpp"

#include <dma.h>

DmaChannel::DmaChannel(unsigned int channel) : channelNumber(channel)
{
	dma_channel_initialize(channelNumber, nullptr, 0);
	dma_channel_fast_waits(channelNumber);
}

DmaChannel::~DmaChannel()
{
	dma_channel_shutdown(channelNumber, 0);
}

DmaChannel& DmaChannel::GIFChannel()
{
	static DmaChannel channel(DMA_CHANNEL_GIF);
	return channel;
}

DmaChannel& DmaChannel::FromIPUChannel()
{
	static DmaChannel channel(DMA_CHANNEL_fromIPU);
	return channel;
}

DmaChannel& DmaChannel::ToIPUChannel()
{
	static DmaChannel channel(DMA_CHANNEL_toIPU);
	return channel;
}
