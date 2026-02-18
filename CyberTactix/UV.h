#pragma once

#include "PixMath.h"

// UV convention used by PixSDLib: (0.0f,0.0f) is top-left, (1.0f,1.0f) is bottom-right. 
// This matches SDL’s top-left origin convention to avoid friction.
// UVQuad and UVRect do not enforce ordering or clamping. Callers are responsible for valid ranges/order if required.
namespace pix
{
	// UVQuad represents 4 explicit texture coordinate points for SpriteMesh.
	// 
	// Philosophy:	
	// UVQuad enables corner-labeled quad mapping that is not necessarily rectangular.
    // The 4 points indicate their intended default location, but they can also be flipped.
	struct UVQuad
	{
		UVQuad(): 
			TopLeft(0.0f, 0.0f), 
			TopRight(1.0f, 0.0f), 
			BottomRight(1.0f, 1.0f), 
			BottomLeft(0.0f, 1.0f)
		{
		}

		UVQuad(Vector2f topLeft, Vector2f topRight, Vector2f bottomRight, Vector2f bottomLeft): 
			TopLeft(topLeft), 
			TopRight(topRight), 
			BottomRight(bottomRight), 
			BottomLeft(bottomLeft)
		{
		}

		Vector2f TopLeft;
		Vector2f TopRight;
		Vector2f BottomRight;
		Vector2f BottomLeft;
	};

	// UVRect defines a rectangular texture coordinate region by two points (TopLeft and BottomRight). 
	// 
	// Philosophy:
	// UVRect represents a rectangular region by two corner points. This avoids width/height conversions.
	struct UVRect
	{
		UVRect() : 
			TopLeft(0.0f, 0.0f), 
			BottomRight(1.0f, 1.0f) 
		{
		}

		UVRect(Vector2f topLeft, Vector2f bottomRight): 
			TopLeft(topLeft), 
			BottomRight(bottomRight) 
		{
		}

		UVRect(float left, float top, float right, float bottom): 
			TopLeft(left, top), 
			BottomRight(right, bottom) 
		{
		}

		Vector2f GetTopRight() const { return Vector2f(BottomRight.X, TopLeft.Y); }
		
		Vector2f GetBottomLeft() const { return Vector2f(TopLeft.X, BottomRight.Y); }

		Vector2f TopLeft;
		Vector2f BottomRight;
	};

}
