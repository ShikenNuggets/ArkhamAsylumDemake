#include "MoviePlayer.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "Debug.hpp"

MoviePlayer::MoviePlayer(const char* moviePath, int16_t numFrames_) : path(moviePath), currentFrame(nullptr), dupeThisFrame(true), currentFrameIdx(1), numFrames(numFrames_){
	std::string frameName = path + std::string(".raw");
	file = std::ifstream(frameName, std::ios::binary);
	if(!file.is_open()){
		LOG_ERROR("Could not open %s for reading!", frameName);
		return;
	}

	BindCurrentFrame();
}

MoviePlayer::~MoviePlayer(){
	delete currentFrame;
}

void MoviePlayer::Update(){
	if(currentFrameIdx >= numFrames){
		return;
	}
	
	if(dupeThisFrame){
		dupeThisFrame = false;
		return;
	}

	currentFrameIdx++;
	BindCurrentFrame();
	dupeThisFrame = true;
}

void MoviePlayer::BindCurrentFrame(){
	if(!file.is_open()){
		LOG_ERROR("Cutscene file is not available, skipping frame");
		return;
	}

	constexpr int frameSize = 196'608;

	buffer.clear();
	buffer.resize(frameSize);
	
	file.seekg((currentFrameIdx - 1) * frameSize);
	file.read(reinterpret_cast<char*>(buffer.data()), frameSize);

	if(currentFrame == nullptr){
		currentFrame = new TextureBuffer(256, 256, buffer.data());
	}else{
		currentFrame->LoadNewTexture(256, 256, buffer.data());
	}
}
