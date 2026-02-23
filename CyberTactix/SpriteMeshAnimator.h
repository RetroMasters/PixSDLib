#pragma once

#include <vector>
#include "SpriteMesh.h"
#include "UV.h"

namespace pix
{
	// Animation keyframe for the position attributes in SpriteMesh.
	// DurationTicks is the number of update ticks the frame should persist (may be fractional for an average).
	struct SpriteMeshPositionKeyframe
	{
		SpriteMeshPositionKeyframe() = default;

		SpriteMeshPositionKeyframe(Vec2f topLeft, Vec2f topRight, Vec2f bottomRight, Vec2f bottomLeft, float durationTicks) :
			Positions{ topLeft, topRight, bottomRight, bottomLeft },
			DurationTicks(durationTicks)
		{
		}

		Vec2f Positions[SpriteMesh::VERTEX_COUNT];

		float DurationTicks; 
	};

	// Animation keyframe for the color attributes in SpriteMesh.
	// DurationTicks is the number of update ticks the frame should persist (may be fractional for an average).
	struct SpriteMeshColorKeyframe
	{
		SpriteMeshColorKeyframe() = default;

		SpriteMeshColorKeyframe(SDL_Color topLeft, SDL_Color topRight, SDL_Color bottomRight, SDL_Color bottomLeft, float durationTicks) :
			Colors{ topLeft, topRight, bottomRight, bottomLeft },
			DurationTicks(durationTicks)
		{
		}

		SDL_Color Colors[SpriteMesh::VERTEX_COUNT];

		float DurationTicks;
	};

	// Animation keyframe for the UV attributes in SpriteMesh.
	// DurationTicks is the number of update ticks the frame should persist (may be fractional for an average).
	struct SpriteMeshUVKeyframe
	{
		SpriteMeshUVKeyframe() = default;

		SpriteMeshUVKeyframe(UVRect uvRect, float durationTicks) :
			UVs{ uvRect.TopLeft, uvRect.TopRight(), uvRect.BottomRight, uvRect.BottomLeft()},
			DurationTicks(durationTicks)
		{
		}

		SpriteMeshUVKeyframe(const UVQuad& uvFrame, float durationTicks):
			UVs{uvFrame.UVs[0], uvFrame.UVs[1], uvFrame.UVs[2], uvFrame.UVs[3]},
			DurationTicks(durationTicks)
		{
        }

		Vec2f& TopLeft() { return UVs[0]; }
		Vec2f& TopRight() { return UVs[1]; }
		Vec2f& BottomRight() { return UVs[2]; }
		Vec2f& BottomLeft() { return UVs[3]; }

		const Vec2f& TopLeft()     const { return UVs[0]; }
		const Vec2f& TopRight()    const { return UVs[1]; }
		const Vec2f& BottomRight() const { return UVs[2]; }
		const Vec2f& BottomLeft()  const { return UVs[3]; }

		Vec2f UVs[SpriteMesh::VERTEX_COUNT];

		float DurationTicks;
	};

	// Philosophy:
	// SpriteMeshAnimator offers "flipbook" animations for a SpriteMesh by iterating over a keyframe sequence and applying its state to a mesh.
	// It is running the animation from the start to the end of the sequence, and then loops to the start if looping is enabled. 
	// Different specializations of SpriteMeshAnimator animate different attributes (positions, colors, UVs) of a mesh to keep animations modular and composable.
	// SpriteMeshAnimator does not own the keyframe sequence and the target mesh; their validity is the responsibility of the caller.
	// DurationTicks is expected to be positive, but is processed consistently by a tick accumulator for all values, which means that keyframes with DurationTicks <= 0.0f
	// are advanced immediately. The amount of keyframe advancements per Update() is capped to the sequence size.
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

		// Advances the animation state for the next update tick and (optionally) applies the current frame to the target mesh.
		// Returns false if there is no sequence or the animation is finished; otherwise advances internal state and returns true.
		bool Update() 
		{
			if (!frameSequence_ || IsFinished()) return false; // IsFinished() == false ensures sequenceSize > 0

			const KeyframeType* frame = &((*frameSequence_)[currentFrameIndex_]);

			elapsedTicks_ += speedScale_;

			size_t sequenceSize = frameSequence_->size();

			size_t iterationCount = 0;
			while (elapsedTicks_ >= frame->DurationTicks && (isLooped_ || currentFrameIndex_ < (sequenceSize - 1)))
			{
				elapsedTicks_ -= frame->DurationTicks;
				currentFrameIndex_ = (currentFrameIndex_ + 1) % sequenceSize;
				frame = &((*frameSequence_)[currentFrameIndex_]);

				// Cap for pathological tick durations
				if (++iterationCount >= sequenceSize) 
				{ 
				  elapsedTicks_ = 0.0f; 
				  break;
				} 
			}

			if (targetMesh_) UpdateMesh(*targetMesh_);

			return true;
		}

		// Applies the current keyframe state to the mesh. 
		// This has custom implementations for the specizalizations SpriteMeshPositionAnimator, SpriteMeshColorAnimator, and SpriteMeshUVAnimator. 
		bool UpdateMesh(SpriteMesh& mesh) const  
		{ 
			return false; 
		}

		void Restart() 
		{
			currentFrameIndex_ = 0;
			elapsedTicks_ = 0.0f;

			if (targetMesh_) UpdateMesh(*targetMesh_);
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

		// Elapsed ticks are incremented by speedScale with each Update() call.
		// speedScale is clamped to non-negative values.
		void SetSpeedScale(float speedScale) 
		{
			speedScale_ = speedScale;

			if (speedScale_ < 0.0f)
				speedScale_ = 0.0f;
		}

		// Set or switch to a new keyframe sequence.
		// currentFrameIndex gets clamped to sequence->size() - 1, or to zero if sequence is null/empty.
		// elapsedTicks is preserved.
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

		// Returns true if sequence is null/empty, or if looping is disabled and the last frame has completed (false otherwise).
		bool IsFinished() const 
		{
			if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return true;

			return !isLooped_ && currentFrameIndex_ + 1 == frameSequence_->size() && elapsedTicks_ >= (*frameSequence_)[currentFrameIndex_].DurationTicks;
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

		// Returns nullptr on failure
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

		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)                                                
			mesh.Vertices[i].Position = currentFrame.Positions[i];

		return true;
	}

	template<>
	inline bool SpriteMeshColorAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshColorKeyframe& currentFrame = (*frameSequence_)[currentFrameIndex_];

		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)
			mesh.Vertices[i].Color = currentFrame.Colors[i];

		return true;
	}

	template<>
	inline bool SpriteMeshUVAnimator::UpdateMesh(SpriteMesh& mesh) const 
	{
		if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return false;

		const SpriteMeshUVKeyframe& currentFrame = (*frameSequence_)[currentFrameIndex_];

		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)
			mesh.Vertices[i].UV = currentFrame.UVs[i];

		return true;
	}

}

