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

	GameLoop::GameLoop(const std::string& companyName, const std::string& appName, const LaunchConfigData& configData):
		updateLoopScheduler_(nullptr),
		deltaTime_(0.0f),
		interpolationAlpha_(1.0f),
		isRunning_(true)
	{
		ErrorLogger& errorLogger = ErrorLogger::Get();
		errorLogger.Init(GetPrefPath(companyName, appName), configData.MaxCountPerError);

		// Windows 10+ feature for crisp, properly scaled graphics on high-DPI monitors. Must be called before SDL_Init()!
		if (!SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2"))
			errorLogger.LogError("GameLoop::GameLoop() - SDL_SetHint() fail", "Failed to set permonitorv2 value!");

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		  errorLogger.LogSDLError("GameLoop::GameLoop() - SDL_Init() fail");

		LaunchConfig::Get().Init(configData);

		if (!Window::Get().Init(configData.LogicalScreenWidth, configData.LogicalScreenHeight, configData.IsFullscreen, appName))
		{
			QuitRunning();  // Having no window is a fatal error
			return;
		}

		if (!Renderer::Get().Init(configData.LogicalScreenWidth, configData.LogicalScreenHeight, configData.IsIntegerScale, configData.IsLinearFilter, configData.IsVsync))
		{
			QuitRunning(); // Having no renderer is a fatal error
			return;
		}

		int imgFlags = IMG_Init(IMG_INIT_PNG);
		if ((imgFlags & IMG_INIT_PNG) == 0)
			errorLogger.LogError("GameLoop::GameLoop() - IMG_Init(IMG_INIT_PNG) fail", "Support for PNG images could not be loaded!");
		
		if (TTF_Init() != 0)
			errorLogger.LogSDLError("GameLop::GameLoop() - TTF_Init() fail");

		Audio::Get().Init();

		int soundChannelCount = Audio::Get().ReallocChannels(configData.SoundChannelCount);
		if (soundChannelCount != configData.SoundChannelCount)
			errorLogger.LogError("GameLoop::GameLoop() - Audio::ReallocChannels() fail", "Allocated " + std::to_string(soundChannelCount) + " instead of" + std::to_string(configData.SoundChannelCount) + "!");

		Audio::Get().SetMasterVolume(configData.MasterVolume);
		Audio::Get().SetChannelVolume(configData.SoundVolume);
		Audio::Get().SetMusicVolume(configData.MusicVolume);

		//SetUpdateLoopScheduler((AbstractUpdateLoopScheduler*) new HysteresisUpdateLoopScheduler(configData.UpdatesPerSecond, 0.5f, 60, 3));
		SetUpdateLoopScheduler(nullptr);

		//Gamepad::addGamepadsFromFile("gamecontrollerdb.txt");
		GamepadInput::Get().AddAllGamepads();

		if (SDL_ShowCursor(SDL_DISABLE) < 0)
			errorLogger.LogSDLError("GameLoop()::GameLoop() - SDL_ShowCursor(SDL_DISABLE) fail");

		// if(!SteamAPI_Init()) Display::markClosed();
	}

	GameLoop::~GameLoop() 
	{
		GamepadInput::Get().RemoveAllGamepads();

		Audio::Get().DeleteMusic();

		Mix_CloseAudio();
		Mix_Quit();
		TTF_Quit();
		IMG_Quit();

		Renderer::Get().Destroy();

		Window::Get().Destroy();

		SDL_Quit();
	}


	void GameLoop::Run() 
	{
		Uint64 timeStamp = 0;
		Uint64 prevTimeStamp = 0;

		while (isRunning_)
		{
			prevTimeStamp = timeStamp;
			timeStamp = SDL_GetTicks64();
			deltaTime_ = timeStamp - prevTimeStamp; // Lossless conversion to float

			int updateCount = 1;
			if(updateLoopScheduler_)
			  updateCount = updateLoopScheduler_->Update(deltaTime_);

			UpdateInterpolationAlpha();

			for (int i = 0; i < updateCount; i++)
			{
				MouseInput::Get().BeginUpdate();

				HandleEvents();

				MouseInput::Get().Update();

				Update(); // VIRTUAL				
			}

			Render(); // VIRTUAL 

			Renderer::Get().SwapBuffers(); // Vsynced double buffering

			// SDL_Delay(1); //experimental, helps to keep gameloop smooth on integrated cards for unknown reason
		}

		//SteamAPI_Shutdown();
	}





	void GameLoop::SetUpdateLoopScheduler(AbstractUpdateLoopScheduler* updateLoopScheduler)
	{
		updateLoopScheduler_ = updateLoopScheduler;
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

	void GameLoop::QuitRunning() 
	{
		isRunning_ = false;
	}

	bool GameLoop::IsRunning() const
	{
		return isRunning_;
	}


	void GameLoop::HandleEvents()
	{
		SDL_Event  event;

		while (SDL_PollEvent(&event) != 0) // internal input state are updated every time SDL_PollEvent is called, so call it before checking input
		{
			if (event.type == SDL_QUIT)
			{
				QuitRunning();
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

		interpolationAlpha_ = DivideSafe(updateLoopScheduler_->GetUnprocessedTime(),updateLoopScheduler_->GetUpdatePeriod());

		if (interpolationAlpha_ < 0.0f) interpolationAlpha_ = 0.0f;
		else if (interpolationAlpha_ > 1.0f) interpolationAlpha_ = 1.0f;
	}

    // On Windows, the output path might look like this: C:\\Users\\bob\\AppData\\Roaming\\companyName\\appName\\ErrorLog.txt
	std::string GameLoop::GetPrefPath(const std::string& companyName, const std::string& appName) const
	{
		char* prefPath = SDL_GetPrefPath(companyName.c_str(), appName.c_str());
		if (!prefPath) return std::string();

		std::string result = std::string(prefPath);
		SDL_free(prefPath);

		return result;
	}


}
