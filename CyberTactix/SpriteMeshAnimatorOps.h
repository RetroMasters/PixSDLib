#pragma once

#include <SDL_rect.h>
#include <SDL_render.h>
#include<vector>
#include "SpriteMeshAnimator.h"


namespace pix
{

	// Switches the animator to reversedSequence.
	// reversedSequence is typically the current frame sequence in reversed order,
	// but may also contain additional frames.
	// The current frame index and elapsed time within the keyframe are mirrored
	// so that playback continues seamlessly in the opposite direction.
	// If the new sequence is shorter, the frame index is clamped to a valid range.
	// Returns true if the switch was performed (the sequence must be non-empty), false otherwise.
	template<typename KeyframeType>
	inline bool SwitchToReversedSequence(SpriteMeshAnimator<KeyframeType>& animator, const std::vector<KeyframeType>& reversedSequence)
	{
		if (reversedSequence.empty()) return false;

		const KeyframeType* currentFrame = animator.GetCurrentFrame();

		if (!currentFrame) return false; // An existing currentFrame ensures that the animator sequence is not empty

		animator.SetCurrentFrameIndex(animator.GetFrameSequence()->size() - 1 - animator.GetCurrentFrameIndex());
		animator.SetElapsedTicks(currentFrame->TickDuration - animator.GetElapsedTicks());

		animator.SetFrameSequence(&reversedSequence);

		return true;
	}

	// Samples keyframes in "book order": left-to-right, then top-to-bottom.
	// Starts at startRect. Each subsequent frame advances by startRect.w in X.
	// When the next frame would exceed texWidth, sampling continues on the next row:
	//   Y += startRect.h and X is reset to 0.
	// Frames are generated only while the full rectangle fits inside the texture;
	// sampling stops early if no further full frame fits.
	// On failure (invalid inputs), returns an empty sequence.
	// texWidth/texHeight are the texture size in pixels.
	// Each keyframe's tick duration is set to tickDuration.
	// flip flags can be combined (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL) and are applied after sampling the sequence.
	std::vector<SpriteMeshUVKeyframe> GetUVKeyframeSequence(int texWidth, int texHeight, const SDL_Rect& startRect, int frameCount, float tickDuration, SDL_RendererFlip flip = SDL_FLIP_NONE);

	

	// Returns a copy of the reversed sequence  
	template<typename KeyframeType>
	inline std::vector<KeyframeType> GetReversedSequence(const std::vector<KeyframeType>& sequence)
	{
		return std::vector<KeyframeType>(sequence.rbegin(), sequence.rend());
	}

	// Flips the horizontal UV-coordinates of keyFrame
	void FlipUVHorizontal(SpriteMeshUVKeyframe& keyframe);

	// Flips the vertical UV-coordinates of keyFrame
	void FlipUVVertical(SpriteMeshUVKeyframe& keyframe);

}
