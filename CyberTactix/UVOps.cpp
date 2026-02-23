
#include "UVOps.h"

namespace pix
{
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

	UVQuad GetUVQuad(UVRect rect)
	{
		return UVQuad(rect.TopLeft, rect.TopRight(), rect.BottomRight, rect.BottomLeft());
	}

	UVQuad GetUVQuad(int texWidth, int texHeight, SDL_Rect rect)
	{
		return GetUVQuad(GetUVRect(texWidth, texHeight, rect));
	}

	Vec2f GetBoundsSize(const UVQuad& quad)
	{
		Vec2f min(quad.TopLeft());
		Vec2f max(quad.BottomRight());

		if (quad.TopRight().X < min.X) min.X = quad.TopRight().X;
		if (quad.TopRight().Y < min.Y) min.Y = quad.TopRight().Y;
		if (quad.BottomRight().X < min.X) min.X = quad.BottomRight().X;
		if (quad.BottomRight().Y < min.Y) min.Y = quad.BottomRight().Y;
		if (quad.BottomLeft().X < min.X) min.X = quad.BottomLeft().X;
		if (quad.BottomLeft().Y < min.Y) min.Y = quad.BottomLeft().Y;

		if (quad.BottomLeft().X > max.X) max.X = quad.BottomLeft().X;
		if (quad.BottomLeft().Y > max.Y) max.Y = quad.BottomLeft().Y;
		if (quad.TopLeft().X > max.X) max.X = quad.TopLeft().X;
		if (quad.TopLeft().Y > max.Y) max.Y = quad.TopLeft().Y;
		if (quad.TopRight().X > max.X) max.X = quad.TopRight().X;
		if (quad.TopRight().Y > max.Y) max.Y = quad.TopRight().Y;

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