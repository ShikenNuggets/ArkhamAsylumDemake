#include "Mesh.hpp"

#include <iostream>
#include <filesystem>

#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <gs_gp.h>

static constexpr uint64_t red = 0xf0;
static constexpr uint64_t blue = 0x0f;
static constexpr uint64_t green = 0x0f;

static constexpr size_t ModelBufferPrefix = (4 * sizeof(qword_t));

Mesh::Mesh(const char* fileName){
	std::printf("[PS2E] Begin mesh init...\n");

	uint8_t* rawBuffer;
	const size_t bufferLen = LoadFile(fileName, rawBuffer);
	if(bufferLen % 16 != 0){
		std::printf("[PS2E] Invalid buffer length!\n");
		return; // TODO - More sophisticated error handling
	}

	std::printf("[PS2E] Malloc buffer %d...\n", bufferLen + ModelBufferPrefix);
	buffer = static_cast<qword_t*>(std::malloc(bufferLen + ModelBufferPrefix));
	qword_t* q = buffer;

	numVertex = bufferLen / 16;
	//if(numVertex % 3 != 0){
		//throw nullptr;
	//}

	vertexSize = 1;
	vertexPosOffset = 0;
	numFaces = numVertex / 3;
	std::printf("[PS2E] Initializing model: verts = %d, faces = %d, buffer size = %d\n", numVertex, numFaces, bufferLen);

	// Create GIFTag, set registers via A+D
	q->dw[0] = 0x1000000000000002;
	dw[1] = 0x000000000000000e;
	q++;

	// Set PRIM = Triangle
	q->dw[0] = GS_SET_PRIM(GS_PRIM_TRIANGLE, 0, 0, 0, 0, 0, 0, 0, 0);
	q->dw[1] = GS_REG_PRIM;
	q++;

	// Set RGBAQ
	q->dw[0] = GS_SET_RGBAQ(red, green, blue, 0x80, 0x80);
	q->dw[1] = GS_REG_RGBAQ;
	q++;

	q->dw[0] = 0x1000000000000000 | (numFaces & 0x3fff);
	q->dw[1] = 0x0000000000000555;
	q++;

	std::memcpy(q, rawBuffer, bufferLen);
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
		return 0;
	}

	int len = std::ftell(f);

	if(std::fseek(f, 0, SEEK_SET) != 0){
		std::printf("[PS2E] File seek failed! Error: %s\n", std::strerror(errno));
		return 0;
	}

	outBuffer = static_cast<uint8_t*>(std::malloc(len));
	if(outBuffer == nullptr){
		std::printf("[PS2E] Malloc failed - we are out of memory!\n");
		return 0;
	}

	size_t byteRead = std::fread(outBuffer, 1, len, f);

	if(std::fclose(f) != 0){
		std::printf("[PS2E] Something went wrong when closing the file. Error: %s\n", std::strerror(errno));
	}

	return byteRead;
}