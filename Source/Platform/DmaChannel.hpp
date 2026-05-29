#pragma once

#include <packet.h>

class DmaChannel
{
public:
	DmaChannel(unsigned int channel);
	~DmaChannel();

	void SendNormal(const packet_t* packet);
	void SendChain(const packet_t* packet);

	static DmaChannel& GIFChannel();
	static DmaChannel& FromIPUChannel();
	static DmaChannel& ToIPUChannel();

private:
	unsigned int channelNumber;
};
