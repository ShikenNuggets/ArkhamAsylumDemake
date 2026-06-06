#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <span>
#include <string_view>
#include <thread>
#include <vector>

class FileBuffer
{
public:
	FileBuffer(std::string_view filePath, size_t bufferSize, std::span<const uint8_t> payload = {});
	~FileBuffer();

	FileBuffer(const FileBuffer&) = delete;
	FileBuffer& operator=(const FileBuffer&) = delete;

	std::span<uint8_t> GetChunk() const;

	void Advance(size_t bytes);

	bool IsExhausted() const{ return isExhausted; }

private:
	void WorkerThread();

	// Returns true if there are more bytes to read from the file
	bool FillBuffer();

	size_t bufferSize;
	size_t paddingSize;

	std::array<uint8_t*, 2> buffers;
	std::array<size_t, 2> validBytes;
	std::array<std::atomic<bool>, 2> bufferReady;

	int readIndex;
	size_t readOffset;
	int writeIndex;

	std::ifstream file;
	std::vector<uint8_t> eofPayload;

	bool fileHitEof;
	std::atomic<bool> isExhausted;
	std::atomic<bool> threadActive;
	std::thread worker;
};
