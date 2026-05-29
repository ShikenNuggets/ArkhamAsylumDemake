#pragma once

class DmaChannel
{
public:
	DmaChannel(unsigned int channel);
	~DmaChannel();

	static DmaChannel& GIFChannel();
	static DmaChannel& FromIPUChannel();
	static DmaChannel& ToIPUChannel();

private:
	unsigned int channelNumber;
};
