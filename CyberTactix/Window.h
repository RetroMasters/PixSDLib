#pragma once

#include <SDL.h>
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
	//
	// The desktop resolution is cached once at initialization to reduce overhead for frequent access.
	class Window : private Uncopyable
	{

	public:

		// Returns the Window instance
		static Window& Get();

		
		// Initializes the Window singleton and creates the main window.
        // The initial window mode (windowed or fullscreen) is applied at creation time to avoid visible transitions during startup.
		// Returns true if initialization succeeds or if the Window is already initialized, false otherwise.
		// Can be reinitialized after Destroy().
		bool Init(int width, int height, bool isFullscreen, const std::string& title);

		// Destroys the window resource.
		// Must be called after destroying the Renderer to ensure proper cleanup order.
		void Destroy();



		void SetFullscreen(bool isFullScreen);

		void SetWindowedSize(int width, int height);

		void SetTitle(const std::string& title);



		bool IsFullScreen() const;

		int GetWindowedWidth() const;

		int GetWindowedHeight() const;

		// Returns the horizontal desktop resolution
		int GetScreenWidth() const;

		// Returns the vertical desktop resolution
		int GetScreenHeight() const;

		const std::string& GetTitle() const;

		SDL_Window* GetSDLWindow() const;

		bool IsInitialized() const;

	private:

		Window();
		~Window();

		SDL_Window* sdlWindow_;

		int windowedWidth_;     // Preferred windowed size, restored when exiting fullscreen
		int windowedHeight_;

		int screenWidth_;       // Desktop resolution (cached once)
		int screenHeight_;

		std::string title_;
		bool isInitialized_;
		
	};

}




