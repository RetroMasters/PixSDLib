#pragma once

#include <vector>
#include "SpriteMesh.h"
#include "UV.h"

namespace pix
{
	// Animation keyframe for the position attributes in SpriteMesh.
	// TickDuration encodes the (average) number of consecutive update ticks the frame should persist. 
	struct SpriteMeshPositionKeyframe
	{
		SpriteMeshPositionKeyframe() = default;

		SpriteMeshPositionKeyframe(Vector2f topLeft, Vector2f topRight, Vector2f bottomRight, Vector2f bottomLeft, float tickDuration) :
			Positions{ topLeft, topRight, bottomRight, bottomLeft },
			TickDuration(tickDuration)
		{
		}

		Vector2f Positions[SpriteMesh::VERTEX_COUNT];

		float TickDuration; 
	};

	// Animation keyframe for the color attributes in SpriteMesh.
	// TickDuration encodes the (average) number of consecutive update ticks the frame should persist. 
	struct SpriteMeshColorKeyframe
	{
		SpriteMeshColorKeyframe() = default;

		SpriteMeshColorKeyframe(SDL_Color topLeft, SDL_Color topRight, SDL_Color bottomRight, SDL_Color bottomLeft, float tickDuration) :
			Colors{ topLeft, topRight, bottomRight, bottomLeft },
			TickDuration(tickDuration)
		{
		}

		SDL_Color Colors[SpriteMesh::VERTEX_COUNT];

		float TickDuration;
	};

	// Animation keyframe for the UV attributes in SpriteMesh.
	// TickDuration encodes the (average) number of consecutive update ticks the frame should persist. 
	struct SpriteMeshUVKeyframe
	{
		SpriteMeshUVKeyframe() = default;

		SpriteMeshUVKeyframe(UVRect uvRect, float tickDuration) :
			UVs{ uvRect.TopLeft, uvRect.TopRight(), uvRect.BottomRight, uvRect.BottomLeft()},
			TickDuration(tickDuration)
		{
		}

		SpriteMeshUVKeyframe(const UVQuad& uvFrame, float tickDuration):
			UVs{uvFrame.UVs[0], uvFrame.UVs[1], uvFrame.UVs[2], uvFrame.UVs[3]},
			TickDuration(tickDuration)
		{
        }

		Vector2f& TopLeft() { return UVs[0]; }
		Vector2f& TopRight() { return UVs[1]; }
		Vector2f& BottomRight() { return UVs[2]; }
		Vector2f& BottomLeft() { return UVs[3]; }

		const Vector2f& TopLeft()     const { return UVs[0]; }
		const Vector2f& TopRight()    const { return UVs[1]; }
		const Vector2f& BottomRight() const { return UVs[2]; }
		const Vector2f& BottomLeft()  const { return UVs[3]; }

		Vector2f UVs[SpriteMesh::VERTEX_COUNT];

		float TickDuration;
	};

	// Philosophy:
	// SpriteMeshAnimator offers "flip book" animations for a SpriteMesh by iterating over a keyframe sequence and applying its state to a mesh.
	// It is running the animation from the start to the end of the sequence, and then loops to the start if looping is enabled. 
	// Different types of the SpriteMeshAnimator animate different attributes (positions, colors, UVs) of a mesh to keep animations modular and composable.
	// SpriteMeshAnimator does not own the keyframe sequence and the target mesh; their validity is the responsibility of the caller.
	// TickDuration is expected to be positive, but is processed consistently for all values, which means that negative TickDurations have an accelerating effect, 
	// and frames with a TickDuration = 0.0f are fast-forwarded until cap. The amount of frame advancements per Update() are capped to the sequence size.
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
		// Returns true when the animation is not finished and the elapsed ticks advance by speed scale, false otherwise.
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
				if (++iterationCount >= sequenceSize) 
				{ 
				  elapsedTicks_ = 0.0f; 
				  break;
				} 
			}

			if (targetMesh_) UpdateMesh(*targetMesh_);

			return true;
		}

		// Applies the current frame state to the mesh (implemented in specializations) in order to animate it
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

		// The tick time is incremented by speedScale (ticks per update).
		// speedScale is clamped to non-negative values.
		void SetSpeedScale(float speedScale) 
		{
			speedScale_ = speedScale;

			if (speedScale_ < 0.0f)
				speedScale_ = 0.0f;
		}

		// Set or switch to a new keyframe sequence.
		// currentFrameIndex gets clamped to sequence->size() - 1, or to zero if sequence is null or empty.
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

		// Returns false if the animation is looped or tick time hasn't advanced to the end of the last frame, returns true otherwise.
		bool IsFinished() const 
		{
			if (!frameSequence_ || currentFrameIndex_ >= frameSequence_->size()) return true;

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
		if (!frameSequence_|| currentFrameIndex_ >= frameSequence_->size()) return false;

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

