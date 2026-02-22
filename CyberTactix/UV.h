#pragma once

#include "PixMath.h"

// UV convention used by PixSDLib: (0.0f, 0.0f) is top-left, (1.0f, 1.0f) is bottom-right.
// This matches SDL’s top-left origin convention to avoid friction.
// UVQuad and UVRect do not enforce ordering or clamping. Callers are responsible for valid ranges/order if required.
namespace pix
{

	// UVQuad represents 4 explicit texture coordinate points.
	//
	// Philosophy:
	// UVQuad enables UV-mapping for SpriteMesh that is not necessarily rectangular.
	// Intended corner order in the array (but values may be flipped): 0=TopLeft, 1=TopRight, 2=BottomRight, 3=BottomLeft.
	struct UVQuad
	{
		static constexpr size_t UV_COUNT = 4;

		UVQuad():
			UVs{ {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} }
		{
		}

		UVQuad(Vector2f topLeft, Vector2f topRight, Vector2f bottomRight, Vector2f bottomLeft) :
			UVs{topLeft, topRight, bottomRight, bottomLeft}
		{
		}
		 
		Vector2f& TopLeft() { return UVs[0]; }
		Vector2f& TopRight() { return UVs[1]; }
		Vector2f& BottomRight() { return UVs[2]; }
		Vector2f& BottomLeft() { return UVs[3]; }

		const Vector2f& TopLeft()     const { return UVs[0]; }
		const Vector2f& TopRight()    const { return UVs[1]; }
		const Vector2f& BottomRight() const { return UVs[2]; }
		const Vector2f& BottomLeft()  const { return UVs[3]; }

		Vector2f UVs[UV_COUNT];
	};



	// UVRect defines a rectangular texture coordinate region by two points (TopLeft and BottomRight).
	//
	// Philosophy:
	// UVRect represents a rectangular region by two corner points for SpriteMesh UV-mapping.
	// This avoids width/height conversions.
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

		Vector2f TopRight() const { return Vector2f(BottomRight.X, TopLeft.Y); }
		
		Vector2f BottomLeft() const { return Vector2f(TopLeft.X, BottomRight.Y); }

		Vector2f TopLeft;
		Vector2f BottomRight;
	};

}
