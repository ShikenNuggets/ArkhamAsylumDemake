#include <vector>

#include <dma.h>
#include <dma_tags.h>
#include <kernel.h>
#include <SDL3/SDL.h>

#include "Debug.hpp"
#include "GameObject.hpp"
#include "Utils.hpp"
#include "Game/Scene.hpp"
#include "Game/SceneManager.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/MoviePlayer3.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/TextureBuffer.hpp"
#include "Input/Input.hpp"

#include "Assets/flowerTexture.h"
#include "_Test/TestScene.hpp"

static constexpr int gScreenWidth = 640;
static constexpr int gScreenHeight = 480;

int main(){
	LOG_INFO("Launching PS2Engine...");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Window* window = SDL_CreateWindow("Arkham Asylum Demake", gScreenWidth, gScreenHeight, 0);
	if (!window)
	{
		LOG_ERROR("Failed to create SDL window: %s", SDL_GetError());
		return -1;
	}

	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, nullptr);
	if (!sdlRenderer)	{
		LOG_ERROR("Failed to create SDL renderer: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		return -1;
	}

	if (!SDL_SetRenderVSync(sdlRenderer, 1))
	{
		LOG_ERROR("Failed to set VSync on SDL renderer: %s", SDL_GetError());
		return -1;
	}

	{
		auto moviePlayer = MoviePlayer3(sdlRenderer);
		moviePlayer.PlayVideo("BmGame/Movies/baa_logo_run_v5_h264.m1v", 640, 360);
	}

	// TODO - Fix the crash that occurs after this
	SleepThread();
	return 0;

	Renderer renderer = Renderer(640, 480);
	renderer.SetClearColor(32, 32, 32);

	TextureBuffer tBuf = TextureBuffer(256, 256, flowerTexture);

	SceneManager sceneManager = SceneManager();
	sceneManager.SwitchScene<TestScene>();

	Input::Init();

	while(true){
		Input::Update();

		auto* currentScene = sceneManager.CurrentScene();
		if(currentScene == nullptr){
			continue;
		}

		currentScene->Update();
		renderer.Render(currentScene->GetCamera(), currentScene->GetGameObjects());
	}
	
	SleepThread(); // Sleep to allow ps2link to take over (TODO: remove this or make it _DEBUG only or something later)
	return 0;
}