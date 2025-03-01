#ifndef PS2E_TEXTURE_BUFFER_H
#define PS2E_TEXTURE_BUFFER_H

#include <cstdint>

#include <draw_buffers.h>

class TextureBuffer{
public:
	TextureBuffer(unsigned int width, unsigned int height, uint8_t* data);
	~TextureBuffer();

	const texbuffer_t* Get() const{ return &texBuffer; }
	texbuffer_t* Get(){ return &texBuffer; }

private:
	texbuffer_t texBuffer;
};

#endif //!PS2E_TEXTURE_BUFFER_H