#include "MoviePlayer.hpp"

#include <array>
#include <fstream>
#include <string>
#include <vector>

#include "Debug.hpp"
#include "ThirdParty/jpgd.hpp"

MoviePlayer::MoviePlayer(const char* moviePath, int16_t numFrames_) : path(moviePath), currentFrame(nullptr), dupeThisFrame(true), currentFrameIdx(1), numFrames(numFrames_), curStreamPos(0), frameData(nullptr){
	std::string frameName = path + std::string(".JPV");
	file = std::ifstream(frameName, std::ios::binary);
	if(!file.is_open()){
		LOG_ERROR("Could not open %s for reading!", frameName);
		return;
	}

	buffer.reserve(8192);

	BindCurrentFrame();
}

MoviePlayer::~MoviePlayer(){
	delete currentFrame;
}

void MoviePlayer::Update(){
	if(currentFrameIdx >= numFrames || !file.is_open()){
		return;
	}
	
	if(dupeThisFrame){
		LoadFrame();
		dupeThisFrame = false;
		return;
	}

	currentFrameIdx++;
	DecompressFrame();
	BindCurrentFrame();
	dupeThisFrame = true;
}

void MoviePlayer::LoadFrame(){
	if(!file.is_open()){
		return;
	}

	buffer.clear();

	std::array<unsigned char, 4> fileSizeByteArray;
	file.seekg(curStreamPos);
	file.read(reinterpret_cast<char*>(fileSizeByteArray.data()), 4);
	ASSERT(file.gcount() <= 4, "Invalid number of bytes read from file!");
	if(file.gcount() < 4){
		file.close();
		return;
	}

	uint32_t fileSize = 0;
	fileSize |= static_cast<uint32_t>(fileSizeByteArray[0]);
	fileSize |= static_cast<uint32_t>(fileSizeByteArray[1]) << 8;
	fileSize |= static_cast<uint32_t>(fileSizeByteArray[2]) << 16;
	fileSize |= static_cast<uint32_t>(fileSizeByteArray[3]) << 24;

	ASSERT(fileSize < buffer.max_size(), "Invalid frame size %u!", fileSize);

	curStreamPos += 4;
	file.seekg(curStreamPos);
	buffer.clear();
	buffer.resize(fileSize);
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
	buffer.resize(file.gcount());
	ASSERT(file.gcount() <= static_cast<int64_t>(fileSize), "Invalid number of bytes read from file!");
	if(buffer.size() < fileSize){
		LOG_WARNING("Something has gone wrong, ending movie playback");
		file.close();
		return;
	}

	curStreamPos += fileSize;
}

void MoviePlayer::DecompressFrame(){
	if(buffer.empty()){
		return;
	}

	std::free(frameData);

	int w;
	int h;
	int actualComps = 0;
	frameData = jpgd::decompress_jpeg_image_from_memory(buffer.data(), buffer.size(), &w, &h, &actualComps, 3);
	if(frameData == nullptr){
		LOG_ERROR("JPEG decompression failed on frame %d!", currentFrameIdx);
		file.close();
		return;
	}
}

void MoviePlayer::BindCurrentFrame(){
	if(frameData == nullptr){
		return;
	}
	
	if(currentFrame == nullptr){
		currentFrame = new TextureBuffer(256, 256, frameData);
	}else{
		currentFrame->LoadNewTexture(256, 256, frameData);
	}
}
