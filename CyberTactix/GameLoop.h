#pragma once

#include<string>
#include "LaunchConfig.h"
#include "UpdateLoopScheduler.h"

namespace pix
{

	// GameLoop is the abstract class for the main game loop.
	//
	// Technical note:
	// SDL events are pumped immediately before each Update() call.
	// Thus, input is polled immediately before each Update() call so simulation always consumes the freshest available physical input.
	// 
	// Philosophy:
	// GameLoop provides an abstract framework for running the game, initializing all PixSDLib singletons. 
	// Concrete game loops implement their custom Update() and Render() method, set the update loop scheduler to their
	// liking, and the whole life cycle and correct order of events is handled by GameLoop.
	class GameLoop
	{
	public:

		GameLoop(const std::string& companyName, const std::string& appName, const LaunchConfigData& configData);
		~GameLoop();

		// Run() is the method where the whole game runs in until QuitRunning() is issued 
		void Run();

		// Update() may be called zero or multiple times per frame, depending on the configured update loop scheduler
		virtual void Update() = 0;

		// Render() is called exactly once per frame
		virtual void Render() = 0;

		// The update-loop scheduler is owned by the caller and can be swapped at runtime.
		// If updateLoopScheduler is nullptr, Update() is called once per frame (variable update loop)
		void SetUpdateLoopScheduler(AbstractUpdateLoopScheduler* updateLoopScheduler);

		// Aborts the Run() loop at the end of the current iteration
		void QuitRunning();

		// Returns the update loop scheduler, or nullptr if none is assigned
		AbstractUpdateLoopScheduler* GetUpdateLoopScheduler() const;
		
		// Returns the time delta between the start of this frame and the last one in milliseconds
		float GetDeltaTime() const;
		
		// Returns the normalized unprocessed time for this frame, after the updates are processed.
		// The returned value is in range [0,1], where value 1 is the normalized update period. 
		// This is used to interpolate visuals between the previous update state and the current one to eliminate visual jitter. 
		float GetInterpolationAlpha() const;
		
		bool IsRunning() const;

		std::string GetPrefPath(const std::string& companyName, const std::string& appName) const;

	private:

		void HandleEvents();
		void UpdateInterpolationAlpha();

		AbstractUpdateLoopScheduler* updateLoopScheduler_;

		float deltaTime_; // in milliseconds
		float interpolationAlpha_;
		bool isRunning_;
	};

}