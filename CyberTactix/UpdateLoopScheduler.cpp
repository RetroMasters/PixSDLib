
#include "UpdateLoopScheduler.h"

namespace pix
{
		AbstractUpdateLoopScheduler::AbstractUpdateLoopScheduler(float updatesPerSecond) 
		{
			SetUpdatesPerSecond(updatesPerSecond);
		}

		void AbstractUpdateLoopScheduler::SetUpdatesPerSecond(float updatesPerSecond)
		{
			updatesPerSecond_ = updatesPerSecond;

			if (updatesPerSecond_ < 0.1f) updatesPerSecond_ = 0.1f; 
		}

		float AbstractUpdateLoopScheduler::GetUpdatesPerSecond() const
		{
			return updatesPerSecond_;
		}

		float AbstractUpdateLoopScheduler::GetUpdatePeriod() const
		{
			return  1000.0f / updatesPerSecond_;
		}


	
		StandardUpdateLoopScheduler::StandardUpdateLoopScheduler(float updatesPerSecond, int maxUpdatesPerFrame) : AbstractUpdateLoopScheduler(updatesPerSecond),
			maxUpdatesPerFrame_(maxUpdatesPerFrame)
		{
			if (maxUpdatesPerFrame_ < 1) maxUpdatesPerFrame_ = 1;
		}

		int StandardUpdateLoopScheduler::Update(float deltaTime)
		{
			unprocessedTime_ += deltaTime;

			const float updatePeriod = GetUpdatePeriod();

			const float maxUnprocessedTime = (maxUpdatesPerFrame_ + 2) * updatePeriod;

			int updateCount = 0;

			if (unprocessedTime_ > maxUnprocessedTime) unprocessedTime_ = maxUnprocessedTime;

			while (unprocessedTime_ >= updatePeriod && updateCount < maxUpdatesPerFrame_)
			{
				updateCount++;
				unprocessedTime_ -= updatePeriod;
			}

			return updateCount;
		}

		float StandardUpdateLoopScheduler::GetUnprocessedTime() const 
		{
			return unprocessedTime_;
		}
	



		HysteresisUpdateLoopScheduler::HysteresisUpdateLoopScheduler(float updatesPerSecond, float noiseWidth, int filterLength, int maxUpdatesPerFrame) : AbstractUpdateLoopScheduler(updatesPerSecond),
			noiseWidth_(noiseWidth),
			filterLength_(filterLength),
			maxUpdatesPerFrame_(maxUpdatesPerFrame)
		{
			if (noiseWidth_ < 0.0f) noiseWidth_ = 0.0f;
			if (filterLength_ < 1) filterLength_ = 1;
			if (maxUpdatesPerFrame_ < 1) maxUpdatesPerFrame_ = 1;

			thresholdOffset_ = 0.5f * noiseWidth_;

			deltaTimeSamples_.reserve(filterLength_);
		}



		int HysteresisUpdateLoopScheduler::Update(float deltaTime)
		{
			const float updatePeriod = GetUpdatePeriod();

			// Keep two additional update periods as buffer so that unprocessedTime_
	        // is not truncated when maxUpdatesPerFrame_ updates are processed on schedule
			const float maxUnprocessedTime = (maxUpdatesPerFrame_ + 2) * updatePeriod;

			if (deltaTime > maxUnprocessedTime) deltaTime = maxUnprocessedTime;

			UpdateAverageDeltaTime(deltaTime);

			unprocessedTime_ += averageDeltaTime_;

			if (unprocessedTime_ > maxUnprocessedTime) unprocessedTime_ = maxUnprocessedTime;

			int updateCount = 0;

			while (true)
			{
				if (IsValueInUpper(unprocessedTime_))
					thresholdOffset_ = -0.5f * noiseWidth_;
				else if (IsValueInLower(unprocessedTime_))
					thresholdOffset_ = 0.5f * noiseWidth_;

				if (unprocessedTime_ < updatePeriod + thresholdOffset_ || updateCount >= maxUpdatesPerFrame_)
					break;

				updateCount++;
				unprocessedTime_ -= updatePeriod;
			}

			return updateCount;
		}

		float HysteresisUpdateLoopScheduler::GetUnprocessedTime() const 
		{
			return unprocessedTime_;
		}

		float HysteresisUpdateLoopScheduler::GetAverageDeltaTime() const
		{
			return averageDeltaTime_;
		}



		bool HysteresisUpdateLoopScheduler::IsValueInUpper(float value) const
		{
			return value >= GetUpdatePeriod() + 0.5f * noiseWidth_ &&
				value < GetUpdatePeriod() + 2.5f * noiseWidth_;
		}

		bool HysteresisUpdateLoopScheduler::IsValueInLower(float value) const
		{
			return value > GetUpdatePeriod() - 2.5f * noiseWidth_ &&
				value < GetUpdatePeriod() - 0.5f * noiseWidth_;
		}

		void HysteresisUpdateLoopScheduler::UpdateAverageDeltaTime(float newDeltaTime)
		{
			// Add newDeltaTime to ring buffer:
			if (deltaTimeSamples_.size() < filterLength_)
				deltaTimeSamples_.push_back(newDeltaTime);
			else
			{
				deltaTimeSamples_[sampleIndex_] = newDeltaTime;
				sampleIndex_++;
				if (sampleIndex_ >= deltaTimeSamples_.size())
					sampleIndex_ = 0;
			}

			// Compute average delta time:
			float deltaTimeSum = 0.0f;
			const int sampleCount = deltaTimeSamples_.size();

			for (int i = 0; i < sampleCount; i++)
				deltaTimeSum += deltaTimeSamples_[i];

			averageDeltaTime_ = deltaTimeSum / sampleCount;
		}

}
