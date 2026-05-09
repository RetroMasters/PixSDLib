#include "GameLoop.h"
#include "ErrorLogger.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Window.h"
#include "Renderer.h"
#include "Audio.h"
#include "Input.h"
#include "PixMath.h"

//#include <steam_api.h>

namespace pix
{

	GameLoop::GameLoop(const std::string& companyName, const std::string& appName, const LaunchConfigData& configData)
	{
		ErrorLogger& errorLogger = ErrorLogger::Get();
		errorLogger.Init(GetPrefPath(companyName, appName), configData.MaxLogCountPerError);

		// Windows 10+ feature for crisp, properly scaled graphics on high-DPI monitors. Must be called before SDL_Init()!
		if (!SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2"))
			errorLogger.LogError("GameLoop::GameLoop() - SDL_SetHint() failure", "Failed to set permonitorv2 value!");

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		  errorLogger.LogSDLError("GameLoop::GameLoop() - SDL_Init() failure");

		LaunchConfig::Get().Init(configData);

		if (!Window::Get().Init(configData.LogicalResolutionWidth, configData.LogicalResolutionHeight, configData.IsFullscreen, appName))
		{
			Quit();  // Having no window is a fatal error
			return;
		}

		if (!Renderer::Get().Init(configData.LogicalResolutionWidth, configData.LogicalResolutionHeight, configData.IsIntegerScale, configData.IsLinearFilter, configData.IsVsync))
		{
			Quit(); // Having no renderer is a fatal error
			return;
		}

		int imgFlags = IMG_Init(IMG_INIT_PNG);
		if ((imgFlags & IMG_INIT_PNG) == 0)
			errorLogger.LogError("GameLoop::GameLoop() - IMG_Init(IMG_INIT_PNG) failure", "Support for PNG images could not be loaded!");
		
		if (TTF_Init() != 0)
			errorLogger.LogSDLError("GameLoop::GameLoop() - TTF_Init() failure");

		Audio::Get().Init();

		int soundChannelCount = Audio::Get().ReallocChannels(configData.SoundChannelCount);
		if (soundChannelCount != configData.SoundChannelCount)
			errorLogger.LogError("GameLoop::GameLoop() - Audio::ReallocChannels() failure", "Allocated " + std::to_string(soundChannelCount) + " instead of " + std::to_string(configData.SoundChannelCount) + "!");

		Audio::Get().SetMasterVolume(configData.MasterVolume);
		Audio::Get().SetChannelVolume(configData.SoundVolume);
		Audio::Get().SetMusicVolume(configData.MusicVolume);

		SetUpdateLoopScheduler(nullptr);

		//Gamepad::addGamepadsFromFile("gamecontrollerdb.txt");
		GamepadInput::Get().AddAllGamepads();

		if (SDL_ShowCursor(SDL_DISABLE) < 0)
			errorLogger.LogSDLError("GameLoop::GameLoop() - SDL_ShowCursor(SDL_DISABLE) failure");

		// if(!SteamAPI_Init()) Display::markClosed();
	}

	GameLoop::~GameLoop() 
	{
		GamepadInput::Get().RemoveAllGamepads();

		Audio::Get().Destroy();

		TTF_Quit();
		IMG_Quit();

		Renderer::Get().Destroy();

		Window::Get().Destroy();

		SDL_Quit();
	}


	void GameLoop::Run() 
	{
		Uint64 timeStamp = SDL_GetTicks64();
		Uint64 prevTimeStamp;

		while (isRunning_)
		{
			prevTimeStamp = timeStamp;
			timeStamp = SDL_GetTicks64();
			deltaTime_ = (float)(timeStamp - prevTimeStamp); // Effectively lossless for normal frame deltas

			int updateCount = 1;
			if (updateLoopScheduler_)
			{
				updateCount = updateLoopScheduler_->Update(deltaTime_);
				updateCount = GetClamped(updateCount, 0, 100); // Final safety cap against invalid scheduler output
			}

			UpdateInterpolationAlpha();

			// Poll current device input state and handle events
			HandleEvents();
			MouseInput::Get().Update();

			for (int i = 0; i < updateCount; i++)
			{
				// Ensure input is pumped for every Update(), not just once per frame, so that Update() gets the freshest device input state available
				if (i > 0)
				{
					SDL_PumpEvents();
					MouseInput::Get().Update();
				}

				Update(); // VIRTUAL				

				MouseInput::Get().EndUpdate();
			}

			Render(); // VIRTUAL 

			MouseInput::Get().EndRender();

			Renderer::Get().SwapBuffers(); // Vsynced double buffering

			// SDL_Delay(1); // Experimental, helps to keep gameloop smooth on some integrated cards for unknown reason
		}

		//SteamAPI_Shutdown();
	}

	void GameLoop::SetUpdateLoopScheduler(AbstractUpdateLoopScheduler* updateLoopScheduler)
	{
		updateLoopScheduler_ = updateLoopScheduler;
	}

	void GameLoop::Quit()
	{
		isRunning_ = false;
	}

	AbstractUpdateLoopScheduler* GameLoop::GetUpdateLoopScheduler() const
	{
		return updateLoopScheduler_;
	}

	float GameLoop::GetDeltaTime() const
	{
		return deltaTime_;
	}

	float GameLoop::GetInterpolationAlpha() const 
	{
		return interpolationAlpha_;
	}

	bool GameLoop::IsRunning() const
	{
		return isRunning_;
	}

	// On Windows, the output path might look like this: C:\\Users\\bob\\AppData\\Roaming\\companyName\\appName\\ErrorLog.txt
	std::string GameLoop::GetPrefPath(const std::string& companyName, const std::string& appName)
	{
		char* prefPath = SDL_GetPrefPath(companyName.c_str(), appName.c_str());
		if (!prefPath) return std::string();

		std::string result = std::string(prefPath);
		SDL_free(prefPath);

		return result;
	}

	void GameLoop::HandleEvents()
	{
		SDL_Event  event;

		// internal input state is updated every time SDL_PollEvent is called, so call it before checking input
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT)
			{
				Quit();
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				MouseInput::Get().HandleEvents(event);
			}
			else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
			{
				GamepadInput::Get().RemoveGamepad(event.cdevice.which);
			}
			else if (event.type == SDL_CONTROLLERDEVICEADDED)
			{
				GamepadInput::Get().AddGamepad(event.cdevice.which);
			}
		}
	}

	void GameLoop::UpdateInterpolationAlpha() 
	{
		if (!updateLoopScheduler_)
		{
			interpolationAlpha_ = 1.0f;
			return;
		}

		interpolationAlpha_ = GetSafeDivision(updateLoopScheduler_->GetUnprocessedTime(),updateLoopScheduler_->GetUpdatePeriod());

		interpolationAlpha_ = GetClamped(interpolationAlpha_, 0.0f, 1.0f);
	}

}
