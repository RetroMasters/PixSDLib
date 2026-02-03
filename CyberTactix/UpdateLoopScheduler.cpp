#pragma once

#include "UpdateLoopScheduler.h"
#include <vector>

namespace pix
{
		AbstractUpdateLoopScheduler::AbstractUpdateLoopScheduler(float updatesPerSecond) :
			updatesPerSecond_(updatesPerSecond)
		{
			SetUpdatesPerSecond(updatesPerSecond);
		}

		void AbstractUpdateLoopScheduler::SetUpdatesPerSecond(float updatesPerSecond)
		{
			updatesPerSecond_ = updatesPerSecond;

			if (updatesPerSecond_ < 0.5f) updatesPerSecond_ = 0.5f;
		}

		float AbstractUpdateLoopScheduler::GetUpdatesPerSecond() const
		{
			return updatesPerSecond_;
		}

		float AbstractUpdateLoopScheduler::GetUpdatePeriod() const
		{
			return  1000.0f / updatesPerSecond_;
		}


	
		StandardUpdateLoopScheduler::StandardUpdateLoopScheduler(float updatesPerSecond) : AbstractUpdateLoopScheduler(updatesPerSecond),
			unprocessedTime_(0.0f)
		{
		}

		int StandardUpdateLoopScheduler::Update(float deltaTimeMs)
		{
			unprocessedTime_ += deltaTimeMs;

			int updateCount = 0;
			float updatePeriod = GetUpdatePeriod();

			while (unprocessedTime_ >= updatePeriod)
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
			unprocessedTime_(0.0f),
			thresholdOffset_(0.0f),
			noiseWidth_(noiseWidth),
			filterLength_(filterLength),
			maxUpdatesPerFrame_(maxUpdatesPerFrame),
			averageDeltaTime_(0.0f),
			samplesIterator_(0),
			deltaTimeSamples_()
		{
			if (noiseWidth_ < 0.0f) noiseWidth_ = 0.0f;
			if (filterLength_ < 1) filterLength_ = 1;
			if (maxUpdatesPerFrame_ < 1) maxUpdatesPerFrame_ = 1;

			thresholdOffset_ = 0.5f * noiseWidth_;

			deltaTimeSamples_.reserve(filterLength_);
		}



		int HysteresisUpdateLoopScheduler::Update(float deltaTimeMs)
		{
			float updatePeriod = GetUpdatePeriod();

			// Have few additional update periods as buffer so that unprocessedTime_
			// is not truncated to zero (potential stutter otherwise) when maxUpdatesPerFrame_ are processed.

			float maxUnprocessedTime = (maxUpdatesPerFrame_ + 2) * updatePeriod;

			if (deltaTimeMs > maxUnprocessedTime) deltaTimeMs = maxUnprocessedTime;

			UpdateAverageDeltaTime(deltaTimeMs);

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
				deltaTimeSamples_[samplesIterator_] = newDeltaTime;
				samplesIterator_++;
				if (samplesIterator_ >= deltaTimeSamples_.size())
					samplesIterator_ = 0;
			}

			// Compute average delta time:
			float deltaTimeSum = 0.0f;
			for (int i = 0; i < deltaTimeSamples_.size(); i++)
				deltaTimeSum += deltaTimeSamples_[i];

			averageDeltaTime_ = deltaTimeSum / deltaTimeSamples_.size();
		}

}
