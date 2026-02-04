#pragma once

#include <vector>
#include "Texture.h"
//#include "SpriteMesh.h"
#include "PixMath.h"
//#include "MathTypes3D.h"

namespace pix
{
	struct Vertex2D
	{
		Vertex2D()  : Position(0.0f, 0.0f), Color{ 255, 255, 255, 255 }, UV(0.0f, 0.0f) {}

		Vertex2D(const Vector2f& position, const SDL_Color& color, const Vector2f& uv)  :
			Position(position),
			Color(color),
			UV(uv)
		{
		}

		Vector2f  Position;
		SDL_Color Color;
		Vector2f  UV;
	};

	struct SpriteMesh
	{
		SpriteMesh()  = default;

		SpriteMesh(const Vertex2D& topLeft, const Vertex2D& topRight, const Vertex2D& bottomRight, const Vertex2D& bottomLeft)  :
			Vertices{ topLeft, topRight, bottomRight, bottomLeft }
		{
		}

		Vertex2D Vertices[4];

		Vertex2D& TopLeft()   { return Vertices[0]; }
		Vertex2D& TopRight()   { return Vertices[1]; }
		Vertex2D& BottomRight()   { return Vertices[2]; }
		Vertex2D& BottomLeft()  { return Vertices[3]; }

		const Vertex2D& TopLeft()     const  { return Vertices[0]; }
		const Vertex2D& TopRight()    const  { return Vertices[1]; }
		const Vertex2D& BottomRight() const  { return Vertices[2]; }
		const Vertex2D& BottomLeft()  const  { return Vertices[3]; }

		const Texture* MeshTexture = nullptr; //TODO: remove
	};

}
