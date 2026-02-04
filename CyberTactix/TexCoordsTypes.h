#pragma once

#include "PixMath.h"

namespace pix
{

	struct UVQuad
	{
		UVQuad()  : TopLeft(0.0f, 0.0f), TopRight(1.0f, 0.0f), BottomRight(1.0f, 1.0f), BottomLeft(0.0f, 1.0f)
		{
		}

		UVQuad(Vector2f topLeft, Vector2f topRight, Vector2f bottomRight, Vector2f bottomLeft) 
			: TopLeft(topLeft), TopRight(topRight), BottomRight(bottomRight), BottomLeft(bottomLeft)
		{
		}

		Vector2f TopLeft;
		Vector2f TopRight;
		Vector2f BottomRight;
		Vector2f BottomLeft;
	};

	struct UVRect
	{
		UVRect()  = default;
		UVRect(Vector2f topLeft, Vector2f bottomRight)  : TopLeft(topLeft), BottomRight(bottomRight) {}
		UVRect(float left, float top, float right, float bottom)  : TopLeft(left, top), BottomRight(right, bottom) {}

		Vector2f GetSize() const { return BottomRight - TopLeft; }

		Vector2f GetTopRight() { return Vector2f(BottomRight.X, TopLeft.Y); }
		Vector2f GetBottomLeft() { return Vector2f(TopLeft.X, BottomRight.Y); }

		Vector2f TopLeft;
		Vector2f BottomRight;
	};

	struct PixelRect
	{
		PixelRect()  : Position(0.0f, 0.0f), Width(0.0f), Height(0.0f) {}
		PixelRect(Vector2f position, float width, float height)  : Position(position), Width(width), Height(height) {}
		PixelRect(float x, float y, float width, float height)  : Position(x, y), Width(width), Height(height) {}

		Vector2f GetTopRight() const  { return Vector2f(Position.X + Width, Position.Y); }
		Vector2f GetBottomLeft() const  { return Vector2f(Position.X, Position.Y + Height); }
		Vector2f GetBottomRight() const  { return Vector2f(Position.X + Width, Position.Y + Height); }

		Vector2f Position;
		float Width;
		float Height;
	};

}
