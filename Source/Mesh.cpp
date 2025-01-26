#include "Mesh.hpp"

#include <iostream>
#include <filesystem>

#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <gs_gp.h>

static constexpr uint64_t red = 0xf0;
static constexpr uint64_t blue = 0x0;
static constexpr uint64_t green = 0x0;

static constexpr size_t ModelBufferPrefix = (4 * sizeof(qword_t));

Mesh::Mesh(const char* fileName){
	std::printf("[PS2E] Begin mesh init...\n");

	uint8_t* rawBuffer = nullptr;
	const size_t bLen = LoadFile(fileName, rawBuffer);
	if(rawBuffer == nullptr || bLen == 0){
		std::printf("[PS2E] Buffer for loading file contents was not allocated successfully!\n");
		return;
	}

	if(bLen % 16 != 0){
		std::printf("[PS2E] Invalid buffer length!\n");
		return; // TODO - More sophisticated error handling
	}

	bufferLen = bLen + ModelBufferPrefix;
	std::printf("[PS2E] Malloc buffer of %d bytes for mesh...\n", bufferLen);
	buffer = static_cast<qword_t*>(std::malloc(bufferLen));
	qword_t* q = buffer;

	numVertex = bLen / 16;
	if(numVertex % 3 != 0){
		std::printf("[PS2E] Models are expected to have 3 vertices for every face!\n");
		return;
	}

	vertexSize = 1;
	vertexPosOffset = 0;
	numFaces = numVertex / 3;
	std::printf("[PS2E] Initializing model: verts = %d, faces = %d, buffer size = %d\n", numVertex, numFaces, bufferLen);

	// Create GIFTag, set registers via A+D
	q->dw[0] = 0x1000000000000002;
	q->dw[1] = 0x000000000000000e;
	q++;

	// Set PRIM = Triangle
	q->dw[0] = GS_SET_PRIM(GS_PRIM_TRIANGLE, 0, 0, 0, 0, 0, 0, 0, 0);
	q->dw[1] = GS_REG_PRIM;
	q++;

	// Set RGBAQ
	q->dw[0] = GS_SET_RGBAQ(red, green, blue, 0x80, 0x80);
	q->dw[1] = GS_REG_RGBAQ;
	q++;

	q->dw[0] = 0x3000000000000000 | (numFaces & 0x3fff);
	q->dw[1] = 0x0000000000000555;
	q++;

	std::memcpy(q, rawBuffer, bLen);
}


size_t Mesh::LoadFile(const char* fileName, uint8_t*& outBuffer){
	if(access(fileName, F_OK) == -1){
		const int err = errno;
		std::printf("[PS2E] File \"%s\" not found! Error: %s\n", fileName, std::strerror(err));
		if(err == EPERM || err == EACCES){
			std::printf("[PS2E] You may need to \"Enable Host Filesystem\" under the emulation settings in PCSX2\n");
		}

		return 0;
	}

	FILE* f = std::fopen(fileName, "rb");
	if(f == nullptr || std::ferror(f)){
		std::printf("[PS2E] File open failed! Error: %s\n", std::strerror(errno));
		return 0;
	}

	if(std::fseek(f, 0, SEEK_END) != 0){
		std::printf("[PS2E] File seek failed! Error: %s\n", std::strerror(errno));
		std::fclose(f);
		return 0;
	}

	int len = std::ftell(f);
	if(len > 310 * 1024){
		std::printf("[PS2E] File too big!\n");
		std::fclose(f);
		return 0;
	}

	if(std::fseek(f, 0, SEEK_SET) != 0){
		std::printf("[PS2E] File seek failed! Error: %s\n", std::strerror(errno));
		std::fclose(f);
		return 0;
	}

	outBuffer = static_cast<uint8_t*>(std::malloc(len));
	if(outBuffer == nullptr){
		std::printf("[PS2E] Malloc failed - we are out of memory!\n");
		std::fclose(f);
		return 0;
	}

	size_t byteRead = std::fread(outBuffer, 1, len, f);

	if(std::fclose(f) != 0){
		std::printf("[PS2E] Something went wrong when closing the file. Error: %s\n", std::strerror(errno));
	}

	return byteRead;
}