#pragma once

#include <string>
#include "LaunchConfig.h"
#include "UpdateLoopScheduler.h"

namespace pix
{

	// GameLoop is the abstract class for the main game loop.
	//
	// Technical note:
	// SDL events are polled and processed at the beginning of each frame.
    // Before each additional fixed Update() in the same frame, SDL_PumpEvents() refreshes SDL's internal input state,
    // so simulation consumes the freshest available physical input.
    // Mouse wheel input is event-based and is processed only during the frame event poll.
	// 
	// Initialization policy:
    // Non-critical subsystem failures are logged but do not abort startup.
    // User code may continue, quit, or let the player decide.
    // Only failures that make running impossible, such as a missing window or renderer, stop the loop.
	// 
	// Philosophy:
	// GameLoop provides an abstract framework for running the game, initializing all PixSDLib singletons. 
	// Concrete game loops implement their custom Update() and Render() method, set the update loop scheduler (non-owning) to their
	// liking, and the whole life cycle and correct order of events is handled by GameLoop.
	class GameLoop
	{
	public:

		static std::string GetPrefPath(const std::string& companyName, const std::string& appName);

		GameLoop(const std::string& companyName, const std::string& appName, const LaunchConfigData& configData);
		virtual ~GameLoop();

		// Runs the game loop until Quit() is called
		void Run();

		// Update() may be called zero or multiple times per frame, depending on the configured update loop scheduler
		virtual void Update() = 0;

		// Render() is called exactly once per frame
		virtual void Render() = 0;

		// Sets the update-loop scheduler. It can be swapped at runtime.
        // GameLoop does not own the scheduler; lifetime is managed by the caller or derived class.
        // If updateLoopScheduler is nullptr, Update() is called once per frame (variable update loop).
		void SetUpdateLoopScheduler(AbstractUpdateLoopScheduler* updateLoopScheduler);

		// Ends the Run() loop at the end of the current iteration
		void Quit();

		// Returns the update loop scheduler, or nullptr if none is assigned
		AbstractUpdateLoopScheduler* GetUpdateLoopScheduler() const;
		
		// Returns the time delta between the start of this frame and the last one in milliseconds
		float GetDeltaTime() const;
		
		// Returns the normalized unprocessed time for this frame. 
		// This value is already computed at the beginning of the frame, before the updates are processed.
		// The returned value is in range [0,1], where value 1 is the normalized update period. 
		// This is used to interpolate visuals between the previous update state and the current one to eliminate visual jitter. 
		float GetInterpolationAlpha() const;
		
		bool IsRunning() const;

	private:

		void HandleEvents();
		void UpdateInterpolationAlpha();

		// Non-owning
		AbstractUpdateLoopScheduler* updateLoopScheduler_ = nullptr;

		float deltaTime_ = 0.0f; // in milliseconds
		float interpolationAlpha_ = 1.0f;
		bool isRunning_ = true;
	};

}