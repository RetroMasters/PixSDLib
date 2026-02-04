
#include "SpriteMeshAnimatorUtils.h"
#include "TexCoordsUtils.h"


namespace pix
{

	std::vector<SpriteMeshUVKeyframe> GetFrameSequence(int texWidth, int texHeight, const PixelRect& startRect, int frameCount, float duration, SDL_RendererFlip flip)
	{
		std::vector<SpriteMeshUVKeyframe> frameSequence;

		if (texWidth < 1 || texHeight < 1 || frameCount < 1 || duration <= 0.0f)
			return frameSequence;

		float delta = 0.000001f;//EPSILON_F;

		SpriteMeshUVKeyframe frame;

		frame.TickDuration = duration;

		UVRect uvRect = GetUVRect(Vector2f(texWidth, texHeight), startRect);

		Vector2f uvRectSize = uvRect.GetSize();


		frame.UV[0] = uvRect.TopLeft;
		frame.UV[1] = uvRect.GetTopRight();
		frame.UV[2] = uvRect.BottomRight;
		frame.UV[3] = uvRect.GetBottomLeft();

		if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL)
			FlipTexCoordsHorizontal(frame);
		else if (flip == SDL_RendererFlip::SDL_FLIP_VERTICAL)
			FlipTexCoordsVertical(frame);

		frameSequence.push_back(frame);

		for (int i = 1; i < frameCount; i++)
		{
			if (frame.UV[2].X < 1.0f - delta)
			{
				frame.UV[0].X += uvRectSize.X;
				frame.UV[1].X += uvRectSize.X;
				frame.UV[2].X += uvRectSize.X;
				frame.UV[3].X += uvRectSize.X;

				frameSequence.push_back(frame);
			}
			else if (frame.UV[2].Y < 1.0f - delta)
			{
				frame.UV[0].X = 0.0f;
				frame.UV[1].X = uvRectSize.X;
				frame.UV[2].X = uvRectSize.X;
				frame.UV[3].X = 0.0f;

				frame.UV[0].Y += uvRectSize.Y;
				frame.UV[1].Y += uvRectSize.Y;
				frame.UV[2].Y += uvRectSize.Y;
				frame.UV[3].Y += uvRectSize.Y;

				frameSequence.push_back(frame);
			}

		}

		return frameSequence;
	}



	


	void FlipTexCoordsHorizontal(SpriteMeshUVKeyframe& frame)
	{
		float tempCoordX = frame.UV[0].X;

		frame.UV[0].X = frame.UV[1].X;
		frame.UV[1].X = tempCoordX;

		tempCoordX = frame.UV[2].X;

		frame.UV[2].X = frame.UV[3].X;
		frame.UV[3].X = tempCoordX;
	}

	void FlipTexCoordsVertical(SpriteMeshUVKeyframe& frame)
	{
		float tempCoordY = frame.UV[0].Y;

		frame.UV[0].Y = frame.UV[3].Y;
		frame.UV[3].Y = tempCoordY;

		tempCoordY = frame.UV[1].Y;

		frame.UV[1].Y = frame.UV[2].Y;
		frame.UV[2].Y = tempCoordY;
	}

}