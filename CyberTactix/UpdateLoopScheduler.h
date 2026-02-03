#pragma once

#include <vector>

namespace pix
{
	// Abstract base class for fixed-timestep update scheduling (can also be used for variable timestepping as a trivial case).
	//
	// Accumulates frame delta time and determines how many updates should be executed for the current frame.			
	// All time values (deltaTime, unprocessed time, update period) are expressed in milliseconds.
	// The returned update count should be used by the game loop to invoke the fixed update logic this many times.
	class AbstractUpdateLoopScheduler
	{
	public:

		explicit AbstractUpdateLoopScheduler(float updatesPerSecond);
		virtual ~AbstractUpdateLoopScheduler() = default;

		// Accumulates elapsed time and determines how many updates should be processed.
		// deltaTime is the time delta between the start of this frame and the last one in milliseconds.
		// Returns the number of updates to execute this frame.
		virtual int Update(float deltaTimeMs) = 0;

		void SetUpdatesPerSecond(float updatesPerSecond);

		// Returns the remaining time that is not simulated yet in milliseconds
		virtual float GetUnprocessedTime() const = 0;

		float GetUpdatesPerSecond() const;
			
		// Returns the update period in milliseconds
		float GetUpdatePeriod() const;

	private:

		float updatesPerSecond_;
	};



	// This is the classic fixed-timestep scheduler. It is pretty simple, but prone to produce update jitter.
	class StandardUpdateLoopScheduler : public AbstractUpdateLoopScheduler
	{
	public:

		explicit StandardUpdateLoopScheduler(float updatesPerSecond);
		
		virtual ~StandardUpdateLoopScheduler() = default;

		int Update(float deltaTimeMs) override;

		float GetUnprocessedTime() const override;

	private:

		float unprocessedTime_;
	};


	// A stable fixed-timestep scheduler with hysteresis and delta-time filtering.
    // Designed to reduce update jitter caused by noisy frame times.
	class HysteresisUpdateLoopScheduler : public AbstractUpdateLoopScheduler
	{
	public:

		HysteresisUpdateLoopScheduler(float updatesPerSecond, float noiseWidth, int filterLength, int maxUpdatesPerFrame);

		virtual ~HysteresisUpdateLoopScheduler() = default;

		int Update(float deltaTimeMs) override;
		
		float GetUnprocessedTime() const override;
	
		float GetAverageDeltaTime() const;

	private:

		bool IsValueInUpper(float value) const;
	
		bool IsValueInLower(float value) const;

		void UpdateAverageDeltaTime(float newDeltaTime);
		

		float unprocessedTime_;
		float thresholdOffset_;
		float noiseWidth_;
		int   filterLength_;
		int   maxUpdatesPerFrame_;

		float averageDeltaTime_;
		int   samplesIterator_ = 0; // For ring buffer implementation
		std::vector<float> deltaTimeSamples_;

	};


}
