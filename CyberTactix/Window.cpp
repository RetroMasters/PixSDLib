#include "Window.h"
#include "ErrorLogger.h"

namespace pix
{
	Window& Window::Get()
	{
		static Window window_;

		return window_;
	}



	bool Window::Init(int windowedWidth, int windowedHeight, bool isFullscreen, const std::string& title)
	{
		if (isInitialized_) return true;

		if (windowedWidth < 1) windowedWidth = 1;
		if (windowedHeight < 1) windowedHeight = 1;

		Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;

		if (isFullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		sdlWindow_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowedWidth, windowedHeight, flags);
		if (!sdlWindow_)
		{
			ErrorLogger::Get().LogSDLError("Window::Init() - SDL_CreateWindow() failure");
			return false;
		}

		// Try to get the desktop resolution of the main monitor
		SDL_DisplayMode desktopMode;
		if (SDL_GetDesktopDisplayMode(0, &desktopMode) != 0)
		{
			ErrorLogger::Get().LogSDLError("Window::Init() - SDL_GetDesktopDisplayMode() failure");
			screenWidth_ = windowedWidth;
			screenHeight_ = windowedHeight;
		}
		else
		{
		   // Cache the desktop resolution of the main monitor (SDL display 0) 
		   screenWidth_ = desktopMode.w; 
		   screenHeight_ = desktopMode.h;
		}

		windowedWidth_ = windowedWidth;
		windowedHeight_ = windowedHeight;

		title_ = title;
		isInitialized_ = true;

		return true;
	}

	void Window::Destroy()
	{
		if (!sdlWindow_) return;
	
		SDL_DestroyWindow(sdlWindow_);
		sdlWindow_ = nullptr;
		windowedWidth_ = 1;
		windowedHeight_ = 1;
		screenWidth_ = 1;
		screenHeight_ = 1;
		title_ = std::string();
		isInitialized_ = false;
	}



	void Window::SetWindowedSize(int width, int height)
	{
		if (width < 1)  width = 1;
		if (height < 1) height = 1;

		// Only apply immediately if we're in windowed mode
		if (!IsFullscreen())
			SDL_SetWindowSize(sdlWindow_, width, height);

		// Always update the preferred windowed size
		windowedWidth_ = width;
		windowedHeight_ = height;
	}

	void Window::SetFullscreen(bool isFullscreen)
	{
		if (IsFullscreen() == isFullscreen) return;

		if (isFullscreen)
		{
			if (SDL_SetWindowFullscreen(sdlWindow_, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
				ErrorLogger::Get().LogSDLError("Window::SetFullscreen() - SDL_SetWindowFullscreen() failure");
		}
		else
		{
			if (SDL_SetWindowFullscreen(sdlWindow_, 0) != 0)
				ErrorLogger::Get().LogSDLError("Window::SetFullscreen() - SDL_SetWindowFullscreen() failure");
			else
				SDL_SetWindowSize(sdlWindow_, windowedWidth_, windowedHeight_);
		}
	}

	void Window::SetTitle(const std::string& title) 
	{
		SDL_SetWindowTitle(sdlWindow_, title.c_str());
		title_ = title;
	}



	bool Window::IsFullscreen() const
	{
		return (SDL_GetWindowFlags(sdlWindow_) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
	}

	int Window::GetWindowedWidth() const
	{
		return windowedWidth_;
	}

	int Window::GetWindowedHeight() const
	{
		return windowedHeight_;
	}

	int Window::GetScreenWidth() const
	{
		return screenWidth_;
	}

	int Window::GetScreenHeight() const 
	{
		return screenHeight_;
	}

	const std::string& Window::GetTitle() const 
	{
		return title_;
	}

	SDL_Window* Window::GetSDLWindow() const
	{
		return sdlWindow_;
	}

	bool Window::IsInitialized() const 
	{
		return isInitialized_;
	}

	Window::~Window()
	{
		Destroy();
	}

}