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

void DmaChannel::SendNormal(const packet_t* packet)
{
	dma_channel_send_normal(channelNumber, packet->data, packet->qwc, 0, 0);
}

void DmaChannel::SendChain(const packet_t* packet)
{
	dma_channel_send_chain(channelNumber, packet->data, packet->qwc, 0, 0);
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
