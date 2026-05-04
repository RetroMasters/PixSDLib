#include "SpriteMeshAnimatorOps.h"
#include "UVOps.h"


namespace pix
{

	std::vector<SpriteMeshUVKeyframe> GetUVKeyframeSequence(int texWidth, int texHeight, const SDL_Rect& startRect, int frameCount, float tickDuration, SDL_RendererFlip flip)
	{
		std::vector<SpriteMeshUVKeyframe> frameSequence;

		if (!IsRectInTexBounds(texWidth, texHeight, startRect) || startRect.w <= 0 || startRect.h <= 0 || frameCount < 1 || tickDuration <= 0.0f)
			return frameSequence;

		frameSequence.reserve(frameCount);

		SDL_Rect sampleRect = startRect;
	    
		frameSequence.emplace_back(GetUVQuad(texWidth, texHeight, sampleRect), tickDuration);

		for (int i = 1; i < frameCount; i++)
		{
			if (sampleRect.x + (2 * sampleRect.w) <= texWidth) // Is there enough horizontal space for another frame in this row?
			{
				sampleRect.x += sampleRect.w;
			    
				frameSequence.emplace_back(GetUVQuad(texWidth, texHeight, sampleRect), tickDuration);
			}
			else if (sampleRect.y + (2 * sampleRect.h) <= texHeight) // Is there enough vertical space for another frame in the next row?
			{
				sampleRect.x = 0;
				sampleRect.y += sampleRect.h;

				frameSequence.emplace_back(GetUVQuad(texWidth, texHeight, sampleRect), tickDuration);
			}
			else
			{
				break; // No space left for more frames
			}
		}

		if (flip & SDL_RendererFlip::SDL_FLIP_HORIZONTAL)
		{
			const int sequenceSize = frameSequence.size();

			for (int i = 0; i < sequenceSize; i++)
				FlipUVHorizontal(frameSequence[i]);
		}

		if (flip & SDL_RendererFlip::SDL_FLIP_VERTICAL)
		{
			const int sequenceSize = frameSequence.size();

			for (int i = 0; i < sequenceSize; i++)
				FlipUVVertical(frameSequence[i]);
		}

		return frameSequence;
	}

	void FlipUVHorizontal(SpriteMeshUVKeyframe& keyframe)
	{
		float tempX = keyframe.TopLeft().X;

		keyframe.TopLeft().X = keyframe.TopRight().X;
		keyframe.TopRight().X = tempX;

		tempX = keyframe.BottomRight().X;

		keyframe.BottomRight().X = keyframe.BottomLeft().X;
		keyframe.BottomLeft().X = tempX;
	}

	void FlipUVVertical(SpriteMeshUVKeyframe& keyframe)
	{
		float tempY = keyframe.TopLeft().Y;

		keyframe.TopLeft().Y = keyframe.BottomLeft().Y;
		keyframe.BottomLeft().Y = tempY;

		tempY = keyframe.TopRight().Y;

		keyframe.TopRight().Y = keyframe.BottomRight().Y;
		keyframe.BottomRight().Y = tempY;
	}

}