#pragma once

#include <SDL_video.h>
#include <string>
#include "Uncopyable.h"

namespace pix
{

	// The Window singleton manages the lifetime, state, and configuration of the main SDL window,
	// providing controlled access to resizing and fullscreen/windowed mode transitions.
	//
	// Philosophy:
	// This Window abstraction is intentionally minimal and policy-driven.
	// It provides a single main window with basic support for resizing and
	// toggling between windowed mode and borderless desktop fullscreen.
	//
	// Rendering is assumed to operate at a fixed internal resolution which
	// is simply upscaled to the native desktop resolution. 
	// Fullscreen is offered only as a borderless window, and
	// the legacy exclusive fullscreen mode is deliberately not offered.
	// The main monitor's desktop resolution is cached at initialization for simple, stable access.
	// 
	// Window is a critical subsystem; after Init() failure, normal engine execution should not continue.
    // Window methods therefore assume successful initialization and do not perform repeated initialization checks.
	class Window : private Uncopyable
	{

	public:

		// Returns the Window instance
		static Window& Get();

		// Initializes the Window singleton and creates the main window.
        // The initial window mode (windowed or fullscreen) is applied at creation time to avoid visible transitions during startup.
		// Returns true if the SDL_Window was created, or if the Window is already initialized.
		// Window setup failures after creation are logged but do not make Init() fail.
		// Can be reinitialized (not intended) after Destroy().
		bool Init(int windowedWidth, int windowedHeight, bool isFullscreen, const std::string& title);

		// Destroys the window resource.
		// Must be called after destroying the Renderer to ensure proper cleanup order.
		void Destroy();



		void SetFullscreen(bool isFullscreen);

		void SetWindowedSize(int width, int height);

		void SetTitle(const std::string& title);



		bool IsFullscreen() const;

		int GetWindowedWidth() const;

		int GetWindowedHeight() const;

		// Returns the cached desktop width of the main monitor.
		int GetScreenWidth() const;

		// Returns the cached desktop height of the main monitor.
		int GetScreenHeight() const;

		const std::string& GetTitle() const;

		SDL_Window* GetSDLWindow() const;

		bool IsInitialized() const;

	private:

		Window() = default;

		// Final safety cleanup.
        // Prefer explicit Destroy() calls during normal shutdown to control destruction order (Renderer should be destroyed before Window).
		~Window();

		SDL_Window* sdlWindow_ = nullptr;

		int windowedWidth_ = 1;  // Preferred windowed size, restored when exiting fullscreen
		int windowedHeight_ = 1;

		int screenWidth_ = 1;   // Main monitor's desktop resolution, cached at initialization
		int screenHeight_ = 1;

		std::string title_;
		bool isInitialized_ = false;
		
	};

}