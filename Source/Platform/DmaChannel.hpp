#pragma once

#include <cstddef>
#include <cstdint>

#include <packet.h>

class DmaChannel
{
public:
	DmaChannel(unsigned int channel);
	~DmaChannel();

	void Wait();
	void FastWait();

	void SendNormal(const packet_t* packet);
	void SendNormalBytes(uint8_t* data, size_t byteCount);
	void SendChain(const packet_t* packet);

	static DmaChannel& GIFChannel();
	static DmaChannel& FromIPUChannel();
	static DmaChannel& ToIPUChannel();

private:
	unsigned int channelNumber;
};
