#pragma once

#include <SDL_rect.h>
#include <SDL_render.h>
#include <vector>
#include "SpriteMeshAnimator.h"


namespace pix
{

	// Makes the animator switch to the reversedSequence, which is meant to be a copy of its current sequence in reverse.
	// The animator's animation state is adjusted respectively so that the animation can advance continously, but in reverse direction.
	template<typename KeyframeType>
	inline bool SwitchToReversedSequence(SpriteMeshAnimator<KeyframeType>& animator, const std::vector<KeyframeType>& reversedSequence)
	{
		if (reversedSequence.empty()) return false;

		const KeyframeType* currentFrame = animator.GetCurrentFrame();

		if (!currentFrame) return false;

		animator.SetCurrentFrameIndex(animator.GetFrameSequence()->size() - 1 - animator.GetCurrentFrameIndex());
		animator.SetElapsedTime(currentFrame.Duration - animator.GetElapsedTicks());

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
	std::vector<SpriteMeshUVKeyframe> GetUVKeyframeSequence(int texWidth, int texHeight, const SDL_Rect& startRect, int frameCount, float tickDuration, SDL_RendererFlip flip = SDL_FLIP_NONE);

	

	// Returns a copy of the reversed sequence  
	template<typename KeyframeType>
	inline std::vector<KeyframeType> GetReversedSequence(const std::vector<KeyframeType>& sequence)
	{
		if (sequence.empty()) return sequence;

		std::vector<KeyframeType> reversedSequence(sequence.rbegin(), sequence.rend());

		return reversedSequence;
	}

	// Flips the horizontal UV-coordinates of keyFrame
	void FlipUVHorizontal(SpriteMeshUVKeyframe& keyFrame);

	// Flips the vertical UV-coordinates of keyFrame
	void FlipUVVertical(SpriteMeshUVKeyframe& keyFrame);

}
