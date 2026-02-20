
#include "UVOps.h"

namespace pix
{
	UVRect GetUVRect(Vector2f texSize, SDL_Rect rect)
	{
		if (texSize.X <= 0.0f || texSize.Y <= 0.0f)
			return UVRect(0.0f, 0.0f, 0.0f, 0.0f);

		const Vector2f topLeft((float)rect.x, (float)rect.y);
		const Vector2f bottomRight((float)(rect.x + rect.w), (float)(rect.y + rect.h));

		return UVRect(topLeft / texSize, bottomRight / texSize);
	}

	Vector2f GetSize(UVRect rect)
	{
		Vector2f size = rect.BottomRight - rect.TopLeft;

		if (size.X < 0.0f) size.X = -size.X;
		if (size.Y < 0.0f) size.Y = -size.Y;

		return size;
	}

	UVRect GetFlippedX(UVRect rect)
	{
		return UVRect(rect.GetTopRight(), rect.GetBottomLeft());
	}

	UVRect GetFlippedY(UVRect rect)
	{
		return UVRect(rect.GetBottomLeft(), rect.GetTopRight());
	}

	UVRect GetFlippedXY(UVRect rect)
	{
		return UVRect(rect.BottomRight, rect.TopLeft);
	}

	UVQuad GetUVQuad(UVRect rect)
	{
		return UVQuad(rect.TopLeft, rect.GetTopRight(), rect.BottomRight, rect.GetBottomLeft());
	}

	UVQuad GetUVQuad(Vector2f texSize, SDL_Rect rect)
	{
		return GetUVQuad(GetUVRect(texSize, rect));
	}

	Vector2f GetBoundsSize(const UVQuad& quad)
	{
		Vector2f min(quad.TopLeft);
		Vector2f max(quad.BottomRight);

		if (quad.TopRight.X < min.X) min.X = quad.TopRight.X;
		if (quad.TopRight.Y < min.Y) min.Y = quad.TopRight.Y;
		if (quad.BottomRight.X < min.X) min.X = quad.BottomRight.X;
		if (quad.BottomRight.Y < min.Y) min.Y = quad.BottomRight.Y;
		if (quad.BottomLeft.X < min.X) min.X = quad.BottomLeft.X;
		if (quad.BottomLeft.Y < min.Y) min.Y = quad.BottomLeft.Y;

		if (quad.BottomLeft.X > max.X) max.X = quad.BottomLeft.X;
		if (quad.BottomLeft.Y > max.Y) max.Y = quad.BottomLeft.Y;
		if (quad.TopLeft.X > max.X) max.X = quad.TopLeft.X;
		if (quad.TopLeft.Y > max.Y) max.Y = quad.TopLeft.Y;
		if (quad.TopRight.X > max.X) max.X = quad.TopRight.X;
		if (quad.TopRight.Y > max.Y) max.Y = quad.TopRight.Y;

		return max - min;
	}

	UVQuad GetFlippedX(const UVQuad& quad)
	{
		return UVQuad(quad.TopRight, quad.TopLeft, quad.BottomLeft, quad.BottomRight);
	}

	UVQuad GetFlippedY(const UVQuad& quad)
	{
		return UVQuad(quad.BottomLeft, quad.BottomRight, quad.TopRight, quad.TopLeft);
	}

	UVQuad GetFlippedXY(const UVQuad& quad)
	{
		return UVQuad(quad.BottomRight, quad.BottomLeft, quad.TopLeft, quad.TopRight);
	}
}