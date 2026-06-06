#include "FileBuffer.hpp"

#include <cstring>
#include <malloc.h>

#include "Debug.hpp"

FileBuffer::FileBuffer(std::string_view filePath, size_t bufferSize, std::span<const uint8_t> endPayload)
	: bufferSize(bufferSize), paddingSize(endPayload.size()), buffers{{ nullptr, nullptr }}, validBytes{{ 0, 0 }}, bufferReady{{ false, false }},
	  readIndex(0), readOffset(0), writeIndex(0), file(filePath.data(), std::ios::binary | std::ios::ate), fileHitEof(false), isExhausted(false),
	  threadActive(true)
{
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open file: %s", filePath.data());
		fileHitEof = true;
		isExhausted = true;
	}

	buffers[0] = static_cast<uint8_t*>(memalign(64, bufferSize + paddingSize));
	buffers[1] = static_cast<uint8_t*>(memalign(64, bufferSize + paddingSize));

	if (!endPayload.empty())
	{
		eofPayload.assign(endPayload.begin(), endPayload.end());
	}

	file.seekg(0, std::ios::beg);

	// Pre-fill both buffers
	FillBuffer();
	FillBuffer();

	threadActive = true;
	worker = std::thread(&FileBuffer::WorkerThread, this);
}

FileBuffer::~FileBuffer()
{
	threadActive = false;
	if (worker.joinable())
	{
		worker.join();
	}

	if (file.is_open())
	{
		file.close();
	}

	free(buffers[0]);
	free(buffers[1]);
}

std::span<uint8_t> FileBuffer::GetChunk() const
{
	if (!bufferReady[readIndex])
	{
		return {};
	}

	size_t remainingSize = validBytes[readIndex] - readOffset;
	return std::span<uint8_t>(buffers[readIndex] + readOffset, remainingSize);
}

void FileBuffer::Advance(size_t bytes)
{
	if (!bufferReady[readIndex])
	{
		return;
	}

	readOffset += bytes;
	if (readOffset >= validBytes[readIndex])
	{
		bufferReady[readIndex] = false;
		readIndex = (readIndex + 1) % 2; // Switch to the other buffer
		readOffset = 0;

		if (fileHitEof && !bufferReady[readIndex])
		{
			isExhausted = true;
		}
	}
}

void FileBuffer::WorkerThread()
{
	while (threadActive)
	{
		if (!FillBuffer())
		{
			// Both buffers are full or we hit EOF, sleep to give time back to the main process
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}	
}

bool FileBuffer::FillBuffer()
{
	if (fileHitEof)
	{
		return false;
	}

	if (bufferReady[writeIndex])
	{
		return false;
	}

	file.read(reinterpret_cast<char*>(buffers[writeIndex]), bufferSize);
	const auto bytesRead = file.gcount();
	validBytes[writeIndex] = static_cast<size_t>(bytesRead);

	const bool isEof = file.eof() || bytesRead < bufferSize;
	if (isEof)
	{
		fileHitEof = true;
		if (!eofPayload.empty() && eofPayload.size() <= paddingSize)
		{
			uint8_t* dest = buffers[writeIndex] + validBytes[writeIndex];
			memcpy(dest, eofPayload.data(), eofPayload.size());
			validBytes[writeIndex] += eofPayload.size();
		}
	}
	
	bufferReady[writeIndex] = true;
	writeIndex = (writeIndex + 1) % 2; // Switch to the other buffer

	return isEof;
}
