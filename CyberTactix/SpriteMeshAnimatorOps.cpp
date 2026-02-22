
#include "SpriteMeshAnimatorOps.h"
#include "UVOps.h"


namespace pix
{

	/*
	std::vector<SpriteMeshUVKeyframe> GetUVKeyframeSequence(int texWidth, int texHeight, const SDL_Rect& startRect, int frameCount, float tickDuration, SDL_RendererFlip flip)
	{
		std::vector<SpriteMeshUVKeyframe> frameSequence;

		if (!IsRectInTexBounds(texWidth,texHeight, startRect) || startRect.w <= 0 || startRect.h <= 0 || frameCount < 1 || tickDuration <= 0.0f)
			return frameSequence;

		frameSequence.reserve(frameCount);

		const float epsX = 0.5f / texWidth;  // Half of a row pixel
		const float epsY = 0.5f / texHeight; // Half of a column pixel

		const UVRect uvRect = GetUVRect(Vector2f((float)texWidth, (float)texHeight), startRect);
		const Vector2f uvRectSize = GetSize(uvRect);

		SpriteMeshUVKeyframe frame;
		frame.TickDuration = tickDuration;

		frame.UV[0] = uvRect.TopLeft;
		frame.UV[1] = uvRect.GetTopRight();
		frame.UV[2] = uvRect.BottomRight;
		frame.UV[3] = uvRect.GetBottomLeft();

		frameSequence.push_back(frame);

		for (int i = 1; i < frameCount; i++)
		{
			if (frame.UV[2].X + uvRectSize.X < 1.0f + epsX)
			{
				frame.UV[0].X += uvRectSize.X;
				frame.UV[1].X += uvRectSize.X;
				frame.UV[2].X += uvRectSize.X;
				frame.UV[3].X += uvRectSize.X;

				if (frame.UV[1].X > 1.0f || frame.UV[2].X > 1.0f)
				{
					frame.UV[1].X = 1.0f;
					frame.UV[2].X = 1.0f;
				}

				frameSequence.push_back(frame);
			}
			else if (frame.UV[2].Y + uvRectSize.Y < 1.0f + epsY)
			{
				frame.UV[0].X = 0.0f;
				frame.UV[1].X = uvRectSize.X;
				frame.UV[2].X = uvRectSize.X;
				frame.UV[3].X = 0.0f;

				frame.UV[0].Y += uvRectSize.Y;
				frame.UV[1].Y += uvRectSize.Y;
				frame.UV[2].Y += uvRectSize.Y;
				frame.UV[3].Y += uvRectSize.Y;

				if (frame.UV[2].Y > 1.0f || frame.UV[3].Y > 1.0f)
				{
					frame.UV[2].Y = 1.0f;
					frame.UV[3].Y = 1.0f;
				}

				frameSequence.push_back(frame);
			}
			else
			{
				break; // No space left for more frames
			}
		}

		if (flip & SDL_RendererFlip::SDL_FLIP_HORIZONTAL)
		{
			for(size_t i = 0; i < frameSequence.size(); i++)
			FlipUVHorizontal(frameSequence[i]);
		}

		if (flip & SDL_RendererFlip::SDL_FLIP_VERTICAL)
		{
			for (size_t i = 0; i < frameSequence.size(); i++)
				FlipUVVertical(frameSequence[i]);
		}

		return frameSequence;
	}
	*/

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
			if (sampleRect.x + (2 * sampleRect.w) <= texWidth)
			{
				sampleRect.x += sampleRect.w;
			    
				frameSequence.emplace_back(GetUVQuad(texWidth, texHeight, sampleRect), tickDuration);
			}
			else if (sampleRect.y + (2 * sampleRect.h) <= texHeight)
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




	


	void FlipUVHorizontal(SpriteMeshUVKeyframe& frame)
	{
		float tempX = frame.UVs[0].X;

		frame.UVs[0].X = frame.UVs[1].X;
		frame.UVs[1].X = tempX;

		tempX = frame.UVs[2].X;

		frame.UVs[2].X = frame.UVs[3].X;
		frame.UVs[3].X = tempX;
	}

	void FlipUVVertical(SpriteMeshUVKeyframe& frame)
	{
		float tempY = frame.UVs[0].Y;

		frame.UVs[0].Y = frame.UVs[3].Y;
		frame.UVs[3].Y = tempY;

		tempY = frame.UVs[1].Y;

		frame.UVs[1].Y = frame.UVs[2].Y;
		frame.UVs[2].Y = tempY;
	}

}