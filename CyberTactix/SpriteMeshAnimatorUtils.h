#pragma once

#include <SDL.h>
#include "UV.h"
#include "SpriteMeshAnimator.h"


namespace pix
{

	template<typename KeyframeType>
	inline bool ReverseSequence(SpriteMeshAnimator<KeyframeType>& animator, const std::vector<KeyframeType>* reversedFrames)
	{
		const KeyframeType* currentFrame = animator.GetCurrentFrame();

		if (currentFrame == nullptr || reversedFrames == nullptr) return false;

		animator.SetCurrentFrameIndex(animator.GetFrameSequence()->size() - 1 - animator.GetCurrentFrameIndex());
		animator.SetElapsedTime(currentFrame->Duration - animator.GetElapsedTime());

		animator.SetFrameSequence(reversedFrames);
	}

	// Moves the startRect in the order of reading a book for a frameCount times (from left to right, and top to bottom)
	std::vector <SpriteMeshUVKeyframe> GetFrameSequence(int texWidth, int texHeight, const SDL_Rect& startRect, int frameCount, float duration, SDL_RendererFlip flip = SDL_FLIP_NONE);

	

	template<typename KeyframeType>
	inline std::vector<KeyframeType> GetReversedFrameSequence(const std::vector<KeyframeType>& animation)
	{
		std::vector<KeyframeType> reversedAnimation;

		reversedAnimation.reserve(animation.size());

		int lastIndex = animation.size() - 1;

		for (int i = lastIndex; i >= 0; i--)
			reversedAnimation.push_back(animation[i]);

		return reversedAnimation;
	}

	void FlipTexCoordsHorizontal(SpriteMeshUVKeyframe& frame);

	void FlipTexCoordsVertical(SpriteMeshUVKeyframe& frame);

	

}
