#include "MoviePlayer.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "Debug.hpp"

MoviePlayer::MoviePlayer(const char* moviePath, int16_t numFrames_) : path(moviePath), currentFrame(nullptr), currentFrameIdx(1), numFrames(numFrames_){
	BindCurrentFrame();
}

MoviePlayer::~MoviePlayer(){
	delete currentFrame;
}

void MoviePlayer::Update(){
	if(currentFrameIdx >= numFrames){
		return;
	}

	currentFrameIdx++;
	BindCurrentFrame();
}

void MoviePlayer::BindCurrentFrame(){
	std::string frameName = std::string("host:") + path + "/" + path + "-";
	if(currentFrameIdx < 10){
		frameName += "0000" + std::to_string(currentFrameIdx);
	}else if(currentFrameIdx < 100){
		frameName += "000" + std::to_string(currentFrameIdx);
	}else if(currentFrameIdx < 1'000){
		frameName += "00" + std::to_string(currentFrameIdx);
	}else if(currentFrameIdx < 10'000){
		frameName += "0" + std::to_string(currentFrameIdx);
	}else{
		frameName += std::to_string(currentFrameIdx);
	}

	frameName += ".bmp.raw";

	std::fstream filestream;
	filestream.open(frameName, std::ios::in);

	if(!filestream.is_open()){
		LOG_ERROR("Could not open %s for reading!", frameName.c_str());
	}

	std::ifstream input(frameName, std::ios::binary);
	if(!input.is_open()){
		return;
	}

	//auto data = std::vector<uint8_t>(std::istreambuf_iterator<char>(input), {}); //TODO - This is not particularly efficient
	buffer.clear();
	buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(input), {});

	if(currentFrame != nullptr){
		delete currentFrame;
		currentFrame = nullptr;
	}

	currentFrame = new TextureBuffer(256, 256, buffer.data());
}
