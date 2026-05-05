#pragma once

#include <vector>

namespace pix
{
	// Abstract base class for fixed-timestep update scheduling.
	//
	// Accumulates frame delta time and determines how many updates should be executed for the current frame.	
	// All time values are expressed in milliseconds.		
	// The returned update count should be used by the game loop to invoke the fixed update logic this many times.
	class AbstractUpdateLoopScheduler
	{
	public:

		// updatesPerSecond defines the target update rate
		explicit AbstractUpdateLoopScheduler(float updatesPerSecond);

		virtual ~AbstractUpdateLoopScheduler() = default;

		// Accumulates frame delta time and returns the number of updates to execute this frame.
		virtual int Update(float deltaTime) = 0;

		void SetUpdatesPerSecond(float updatesPerSecond);

		// Returns the remaining time that has not been simulated yet
		virtual float GetUnprocessedTime() const = 0;

		float GetUpdatesPerSecond() const;
			
		// Returns the update period
		float GetUpdatePeriod() const;

	private:

		float updatesPerSecond_;
	};


	// Classic fixed-timestep scheduler with max-update protection. Simple, but prone to update jitter.
	class StandardUpdateLoopScheduler : public AbstractUpdateLoopScheduler
	{
	public:

		// - updatesPerSecond defines the target update rate.
	    // - maxUpdatesPerFrame limits how many fixed updates may be executed during one rendered frame.
		explicit StandardUpdateLoopScheduler(float updatesPerSecond, int maxUpdatesPerFrame = 2);
		
		~StandardUpdateLoopScheduler() override = default;

		int Update(float deltaTime) override;

		float GetUnprocessedTime() const override;

	private:

		int   maxUpdatesPerFrame_ = 2;
		float unprocessedTime_ = 0.0f;
	};


	// A stable fixed-timestep scheduler with hysteresis and delta-time filtering.
    // Designed to reduce update jitter caused by noisy frame times.
	class HysteresisUpdateLoopScheduler : public AbstractUpdateLoopScheduler
	{
	public:

		// - updatesPerSecond defines the target update rate.
        // - noiseWidth defines the hysteresis band around the update threshold, in milliseconds.
        // - filterLength defines how many frame deltas are averaged.
        // - maxUpdatesPerFrame limits how many fixed updates may be executed during one rendered frame.
		HysteresisUpdateLoopScheduler(float updatesPerSecond, float noiseWidth, int filterLength, int maxUpdatesPerFrame = 2);

	    ~HysteresisUpdateLoopScheduler() override = default;

		int Update(float deltaTime) override;
		
		float GetUnprocessedTime() const override;
	
		float GetAverageDeltaTime() const;

	private:

		bool IsValueInUpper(float value) const;
	
		bool IsValueInLower(float value) const;

		void UpdateAverageDeltaTime(float newDeltaTime);
		
		float noiseWidth_; // defines the hysteresis band around the update threshold, in milliseconds.
		int   filterLength_;
		int   maxUpdatesPerFrame_ = 2;
		float unprocessedTime_ = 0.0f;
		float thresholdOffset_ = 0.0f;
		float averageDeltaTime_ = 0.0f;
		int   sampleIndex_ = 0; // For ring buffer implementation
		std::vector<float> deltaTimeSamples_;

	};


}
