#include "MoviePlayer.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "Debug.hpp"

MoviePlayer::MoviePlayer(const char* moviePath, int16_t numFrames_) : path(moviePath), currentFrame(nullptr), dupeThisFrame(true), currentFrameIdx(1), numFrames(numFrames_){
	BindCurrentFrame();
}

MoviePlayer::~MoviePlayer(){
	delete currentFrame;
}

void MoviePlayer::Update(){
	if(currentFrameIdx >= numFrames){
		return;
	}

	currentFrameIdx++; // TODO - Ideally don't skip frames, but this gives us adequate performance for now
	if(dupeThisFrame){
		dupeThisFrame = false;
		return;
	}

	BindCurrentFrame();
	dupeThisFrame = true;
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

	std::ifstream input(frameName, std::ios::binary);
	if(!input.is_open()){
		LOG_ERROR("Could not open %s for reading!", frameName.c_str());
		return;
	}

	buffer.clear();
	buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(input), {});

	if(currentFrame == nullptr){
		currentFrame = new TextureBuffer(256, 256, buffer.data());
	}else{
		currentFrame->LoadNewTexture(256, 256, buffer.data());
	}
}
