#pragma once

#include <vector>
#include "SpriteMesh.h"


namespace pix
{

	struct SpriteMeshPositionKeyframe
	{
		Vector2f Positions[SpriteMesh::VertexCount];

		float TickDuration = 1.0f; // Normalized to the update period, so 1.0f is "one update long"
	};

	struct SpriteMeshColorKeyframe
	{
		SDL_Color Colors[SpriteMesh::VertexCount];

		float TickDuration = 1.0f;
	};

	struct SpriteMeshUVKeyframe
	{
		Vector2f UV[SpriteMesh::VertexCount];

		float TickDuration = 1.0f;
	};


	template<typename KeyframeType> class SpriteMeshAnimator
	{
	public:

		//###################################### INITIALIZATION ###################################

		SpriteMeshAnimator(): 
			frameSequence_(nullptr), 
			targetMesh_(nullptr), 
			elapsedTicks_(0.0f), 
			currentFrameIndex_(0), 
			speedScale_(1.0f), 
			isLooped_(true) 
		{
		}

		SpriteMeshAnimator(const std::vector<KeyframeType>* sequence, SpriteMesh* targetMesh = nullptr): 
			frameSequence_(sequence), 
			targetMesh_(targetMesh), 
			elapsedTicks_(0.0f), 
			currentFrameIndex_(0), 
			speedScale_(1.0f), 
			isLooped_(true) 
		{
		}

		~SpriteMeshAnimator()  = default;

		//###################################### FUNCTIONALITY ###################################

		bool Update() 
		{
			if (!frameSequence_ || IsFinished()) return false; // IsFinished() == false ensures sequenceSize > 0

			const KeyframeType* frame = &((*frameSequence_)[currentFrameIndex_]);

			elapsedTicks_ += speedScale_;

			size_t sequenceSize = frameSequence_->size();

			size_t iterationCount = 0;
			while (elapsedTicks_ >= frame->TickDuration && (isLooped_ || currentFrameIndex_ < (sequenceSize - 1)))
			{
				elapsedTicks_ -= frame->TickDuration;
				currentFrameIndex_ = (currentFrameIndex_ + 1) % sequenceSize;
				frame = &((*frameSequence_)[currentFrameIndex_]);

				// Cap for pathological tick durations
				if (++iterationCount > sequenceSize) 
				{ 
				  elapsedTicks_ = 0.0f; 
				  break;
				} 
			}

			if (targetMesh_) UpdateMesh(*targetMesh_);

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
			if (elapsedTicks < 0.0f) elapsedTicks = 0.0f;

			elapsedTicks_ = elapsedTicks;
		}

		bool SetCurrentFrameIndex(size_t index) 
		{
			if (!frameSequence_ || index >= frameSequence_->size()) return false;

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
			frameSequence_ = sequence;

			if (!frameSequence_ || frameSequence_->empty())
			{
				currentFrameIndex_ = 0;
				return;
			}

			if (currentFrameIndex_ >= frameSequence_->size())
				currentFrameIndex_ = frameSequence_->size() - 1;
		}

		void SetTargetMesh(SpriteMesh* targetMesh)
		{
			targetMesh_ = targetMesh;
		}

		//###################################################################################### GETTERS ############################################################


		bool IsFinished() const 
		{
			if (!frameSequence_ || frameSequence_->empty()) return true;

			return !isLooped_ && currentFrameIndex_ + 1 == frameSequence_->size() && elapsedTicks_ >= (*frameSequence_)[currentFrameIndex_].TickDuration;
		}

		float GetElapsedTicks() const 
		{
			return elapsedTicks_;
		}

		size_t GetCurrentFrameIndex() const 
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
			if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return nullptr;

			return &((*frameSequence_)[currentFrameIndex_]);
		}

		const std::vector<KeyframeType>* GetFrameSequence() const 
		{
			return frameSequence_;
		}

		SpriteMesh* GetTargetMesh() const
		{
			return targetMesh_;
		}

	private:

		const std::vector<KeyframeType>* frameSequence_;
		SpriteMesh* targetMesh_;

		float elapsedTicks_;
		size_t   currentFrameIndex_;
		float speedScale_;
		bool  isLooped_;
	};

	using SpriteMeshPositionAnimator = SpriteMeshAnimator<SpriteMeshPositionKeyframe>;
	using SpriteMeshColorAnimator = SpriteMeshAnimator<SpriteMeshColorKeyframe>;
	using SpriteMeshUVAnimator = SpriteMeshAnimator<SpriteMeshUVKeyframe>;

	template<>
	inline bool SpriteMeshPositionAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshPositionKeyframe& currentFrame = (*frameSequence_)[currentFrameIndex_];

		for (size_t i = 0; i < SpriteMesh::VertexCount; i++)                                                
			mesh.Vertices[i].Position = currentFrame.Positions[i];

		return true;
	}

	template<>
	inline bool SpriteMeshColorAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (!frameSequence_|| currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshColorKeyframe& currentFrame = (*frameSequence_)[currentFrameIndex_];

		for (size_t i = 0; i < SpriteMesh::VertexCount; i++)
			mesh.Vertices[i].Color = currentFrame.Colors[i];

		return true;
	}

	template<>
	inline bool SpriteMeshUVAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshUVKeyframe& currentFrame = (*frameSequence_)[currentFrameIndex_];

		for (size_t i = 0; i < SpriteMesh::VertexCount; i++)
			mesh.Vertices[i].UV = currentFrame.UV[i];

		return true;
	}

}

