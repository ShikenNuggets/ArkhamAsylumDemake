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
	
	if(dupeThisFrame){
		dupeThisFrame = false;
		return;
	}

	currentFrameIdx++;
	BindCurrentFrame();
	dupeThisFrame = true;
}

void MoviePlayer::BindCurrentFrame(){
	std::string frameName = path + std::string(".raw");

	std::ifstream input(frameName, std::ios::binary);
	if(!input.is_open()){
		LOG_ERROR("Could not open %s for reading!", frameName);
		return;
	}

	constexpr int frameSize = 196'608;

	buffer.clear();
	buffer.resize(frameSize);
	//buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(input), {});
	
	input.seekg((currentFrameIdx - 1) * frameSize);
	input.read(reinterpret_cast<char*>(buffer.data()), frameSize);

	if(currentFrame == nullptr){
		currentFrame = new TextureBuffer(256, 256, buffer.data());
	}else{
		currentFrame->LoadNewTexture(256, 256, buffer.data());
	}
}
