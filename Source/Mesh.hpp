#ifndef PS2E_MESH_HPP
#define PS2E_MESH_HPP

#include <cstdint>
#include <stdlib.h>

#include <tamtypes.h>

class Mesh{
public:
	Mesh(const char* fileName);

//private: // TODO - Proper abstraction
	qword_t* buffer;
	size_t bufferLen;
	int numFaces;
	int numVertex;
	int vertexSize;
	int vertexPosOffset;

	size_t LoadFile(const char* fileName, uint8_t*& outBuffer);
};

#endif //!PS2E_MESH_HPP