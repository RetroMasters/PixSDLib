
#include "UVOps.h"

namespace pix
{
	// ########################################### UVRECT OPERATIONS #################################


	UVRect GetUVRect(int texWidth, int texHeight, SDL_Rect rect)
	{
		if (texWidth <= 0 || texHeight <= 0)
			return UVRect(0.0f, 0.0f, 0.0f, 0.0f);

		const Vec2f topLeft((float)rect.x, (float)rect.y);
		const Vec2f bottomRight((float)(rect.x + rect.w), (float)(rect.y + rect.h));
		const Vec2f texSize((float)texWidth, (float)texHeight);

		return UVRect(topLeft / texSize, bottomRight / texSize);
	}

	Vec2f GetSize(UVRect rect)
	{
		Vec2f size = rect.BottomRight - rect.TopLeft;

		if (size.X < 0.0f) size.X = -size.X;
		if (size.Y < 0.0f) size.Y = -size.Y;

		return size;
	}

	UVRect GetFlippedX(UVRect rect)
	{
		return UVRect(rect.TopRight(), rect.BottomLeft());
	}

	UVRect GetFlippedY(UVRect rect)
	{
		return UVRect(rect.BottomLeft(), rect.TopRight());
	}

	UVRect GetFlippedXY(UVRect rect)
	{
		return UVRect(rect.BottomRight, rect.TopLeft);
	}

	bool IsRectInTexBounds(int texWidth, int texHeight, SDL_Rect rect)
	{
		return (texWidth > 0 && texHeight > 0 && rect.w >= 0 && rect.h >= 0 &&
			    rect.x >= 0 && rect.y >= 0 && rect.w <= texWidth - rect.x && rect.h <= texHeight - rect.y);
	}


	// ########################################### UVQUAD OPERATIONS #################################


	UVQuad GetUVQuad(UVRect rect)
	{
		return UVQuad(rect.TopLeft, rect.TopRight(), rect.BottomRight, rect.BottomLeft());
	}

	UVQuad GetUVQuad(int texWidth, int texHeight, SDL_Rect rect)
	{
		return GetUVQuad(GetUVRect(texWidth, texHeight, rect));
	}

	Vec2f GetBoundsSize(const UVQuad& quad) // Compute the size of the axis-aligned bounding box
	{
		const Vec2f topLeft(quad.TopLeft());
		const Vec2f topRight(quad.TopRight());
		const Vec2f bottomRight(quad.BottomRight());
		const Vec2f bottomLeft(quad.BottomLeft());

		Vec2f min(topLeft);
		Vec2f max(bottomRight);

		if (topRight.X < min.X) min.X = topRight.X;
		if (topRight.Y < min.Y) min.Y = topRight.Y;
		if (bottomRight.X < min.X) min.X = bottomRight.X;
		if (bottomRight.Y < min.Y) min.Y = bottomRight.Y;
		if (bottomLeft.X < min.X) min.X = bottomLeft.X;
		if (bottomLeft.Y < min.Y) min.Y = bottomLeft.Y;

		if (bottomLeft.X > max.X) max.X = bottomLeft.X;
		if (bottomLeft.Y > max.Y) max.Y = bottomLeft.Y;
		if (topLeft.X > max.X) max.X = topLeft.X;
		if (topLeft.Y > max.Y) max.Y = topLeft.Y;
		if (topRight.X > max.X) max.X = topRight.X;
		if (topRight.Y > max.Y) max.Y = topRight.Y;

		return max - min;
	}

	UVQuad GetFlippedX(const UVQuad& quad)
	{
		return UVQuad(quad.TopRight(), quad.TopLeft(), quad.BottomLeft(), quad.BottomRight());
	}

	UVQuad GetFlippedY(const UVQuad& quad)
	{
		return UVQuad(quad.BottomLeft(), quad.BottomRight(), quad.TopRight(), quad.TopLeft());
	}

	UVQuad GetFlippedXY(const UVQuad& quad)
	{
		return UVQuad(quad.BottomRight(), quad.BottomLeft(), quad.TopLeft(), quad.TopRight());
	}
}