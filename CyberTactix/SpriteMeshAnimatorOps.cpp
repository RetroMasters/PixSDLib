
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
			if (sampleRect.x + (2 * sampleRect.w) <= texWidth) // Is there space for another frame in this row?
			{
				sampleRect.x += sampleRect.w;
			    
				frameSequence.emplace_back(GetUVQuad(texWidth, texHeight, sampleRect), tickDuration);
			}
			else if (sampleRect.y + (2 * sampleRect.h) <= texHeight) // Is there space for another frame in the column?
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
			for (size_t i = 0; i < frameSequence.size(); i++)
				FlipUVHorizontal(frameSequence[i]);
		}

		if (flip & SDL_RendererFlip::SDL_FLIP_VERTICAL)
		{
			for (size_t i = 0; i < frameSequence.size(); i++)
				FlipUVVertical(frameSequence[i]);
		}

		return frameSequence;
	}

	void FlipUVHorizontal(SpriteMeshUVKeyframe& keyframe)
	{
		float tempX = keyframe.UVs[0].X;

		keyframe.UVs[0].X = keyframe.UVs[1].X;
		keyframe.UVs[1].X = tempX;

		tempX = keyframe.UVs[2].X;

		keyframe.UVs[2].X = keyframe.UVs[3].X;
		keyframe.UVs[3].X = tempX;
	}

	void FlipUVVertical(SpriteMeshUVKeyframe& keyframe)
	{
		float tempY = keyframe.UVs[0].Y;

		keyframe.UVs[0].Y = keyframe.UVs[3].Y;
		keyframe.UVs[3].Y = tempY;

		tempY = keyframe.UVs[1].Y;

		keyframe.UVs[1].Y = keyframe.UVs[2].Y;
		keyframe.UVs[2].Y = tempY;
	}

}