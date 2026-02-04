#pragma once

#include <vector>
#include "SpriteMesh.h"


namespace pix
{

	struct SpriteMeshPositionKeyframe
	{
		Vector2f Positions[4];

		float TickDuration = 0.0f;            //Normalized to the update period, so 1.0f is "one update long"
	};

	struct SpriteMeshColorKeyframe
	{
		SDL_Color Colors[4];

		float TickDuration = 0.0f;
	};

	struct SpriteMeshUVKeyframe
	{
		Vector2f UV[4];

		float TickDuration = 0.0f;
	};


	template<typename KeyframeType> class SpriteMeshAnimator
	{
	public:

		SpriteMeshAnimator()  : frameSequence_(nullptr), elapsedTicks_(-1.0f), currentFrameIndex_(0), speedScale_(1.0f), isLooped_(true) {}
		SpriteMeshAnimator(const std::vector<KeyframeType>* sequence)  : frameSequence_(sequence), elapsedTicks_(-1.0f), currentFrameIndex_(0), speedScale_(1.0f), isLooped_(true) {}
		~SpriteMeshAnimator()  = default;


		bool Update() 
		{
			if (frameSequence_ == nullptr || IsFinished() || currentFrameIndex_ >= frameSequence_->size()) return false;

			const KeyframeType* frame = &((*frameSequence_)[currentFrameIndex_]);

			if (frame->TickDuration <= 0.0f) return false;

			elapsedTicks_ += speedScale_;

			int sequenceSize = frameSequence_->size();

			while (elapsedTicks_ >= frame->TickDuration && (isLooped_ || currentFrameIndex_ < (sequenceSize - 1)))
			{
				elapsedTicks_ -= frame->TickDuration;
				currentFrameIndex_ = (currentFrameIndex_ + 1) % sequenceSize;
				frame = &((*frameSequence_)[currentFrameIndex_]);
			}

			return true;
		}

		bool UpdateMesh(SpriteMesh& mesh) const  { return false; }

		void Restart() 
		{
			currentFrameIndex_ = 0;
			elapsedTicks_ = 0.0f;
		}

		void SetElapsedTicks(float elapsedTicks) 
		{
			elapsedTicks_ = elapsedTicks;
		}

		bool SetCurrentFrameIndex(int index) 
		{
			if (frameSequence_ == nullptr || index >= frameSequence_->size() || index < 0) return false;

			currentFrameIndex_ = index;

			return true;
		}

		void SetIsLooped(bool isLooped) 
		{
			isLooped_ = isLooped;
		}

		void SetSpeedScale(float speedScale) 
		{
			speedScale_ = speedScale;

			if (speedScale_ < 0.0f)
				speedScale_ = 0.0f;
		}

		void SetFrameSequence(const std::vector<KeyframeType>* sequence) 
		{
			if (sequence != nullptr && currentFrameIndex_ >= sequence->size())
				currentFrameIndex_ = sequence->size() - 1;

			frameSequence_ = sequence;
		}


		//###################################################################################### CONST METHODS ############################################################


		bool IsFinished() const 
		{
			return !isLooped_ && currentFrameIndex_ + 1 == frameSequence_->size() && elapsedTicks_ >= (*frameSequence_)[currentFrameIndex_].TickDuration;
		}

		float GetElapsedTicks() const 
		{
			return elapsedTicks_;
		}

		int GetCurrentFrameIndex() const 
		{
			return currentFrameIndex_;
		}

		bool GetIsLooped() const  
		{
			return isLooped_;
		}

		float GetSpeedScale() const 
		{
			return speedScale_;
		}

		const KeyframeType* GetCurrentFrame() const 
		{
			if (frameSequence_ == nullptr || currentFrameIndex_ >= frameSequence_->size()) return nullptr;

			return &((*frameSequence_)[currentFrameIndex_]);
		}

		const std::vector<KeyframeType>* GetFrameSequence() const 
		{
			return frameSequence_;
		}

	private:

		const std::vector<KeyframeType>* frameSequence_;

		float elapsedTicks_;
		int   currentFrameIndex_;
		float speedScale_;
		bool  isLooped_;
	};

	using SpriteMeshPositionAnimator = SpriteMeshAnimator<SpriteMeshPositionKeyframe>;
	using SpriteMeshColorAnimator = SpriteMeshAnimator<SpriteMeshColorKeyframe>;
	using SpriteMeshUVAnimator = SpriteMeshAnimator<SpriteMeshUVKeyframe>;

	template<>
	inline bool SpriteMeshPositionAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (frameSequence_ == nullptr || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshPositionKeyframe* const currentFrame = &((*frameSequence_)[currentFrameIndex_]);

		for (int i = 0; i < 4; i++)                                                  //TODO: do faster memory copy?
			mesh.Vertices[i].Position = currentFrame->Positions[i];

		return true;
	}

	template<>
	inline bool SpriteMeshColorAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (frameSequence_ == nullptr || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshColorKeyframe* const currentFrame = &((*frameSequence_)[currentFrameIndex_]);

		for (int i = 0; i < 4; i++)
			mesh.Vertices[i].Color = currentFrame->Colors[i];

		return true;
	}

	template<>
	inline bool SpriteMeshUVAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (frameSequence_ == nullptr || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshUVKeyframe* const currentFrame = &((*frameSequence_)[currentFrameIndex_]);

		for (int i = 0; i < 4; i++)
			mesh.Vertices[i].UV = currentFrame->UV[i];

		return true;
	}

}

