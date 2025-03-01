#ifndef PS2E_TEXTURE_BUFFER_H
#define PS2E_TEXTURE_BUFFER_H

#include <cstdint>

#include <draw_buffers.h>

class TextureBuffer{
public:
	TextureBuffer(unsigned int width_, unsigned int height_, uint8_t* data);
	~TextureBuffer();

	const texbuffer_t* Get() const{ return &texBuffer; }
	texbuffer_t* Get(){ return &texBuffer; }

	void Bind();

private:
	texbuffer_t texBuffer;
	unsigned int width;
	unsigned int height;
};

#endif //!PS2E_TEXTURE_BUFFER_H