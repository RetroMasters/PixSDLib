#pragma once

#include <SDL.h>
#include "PixMath.h"

namespace pix
{
	// Vertex2D is a 2D vertex that stores a 2D position, Color, and UV-coordinates.
	// 
	// Philosophy:
	// Vertex2D instances are points with attributes that define the SpriteMesh.
	struct Vertex2D
	{
		Vertex2D(): 
			Position(0.0f, 0.0f), 
			Color{ 255, 255, 255, 255 },
			UV(0.0f, 0.0f) 
		{
		}

		Vertex2D(Vector2f position, SDL_Color color, Vector2f uv):
			Position(position),
			Color(color),
			UV(uv)
		{
		}

		Vector2f  Position;
		SDL_Color Color;
		Vector2f  UV;
	};

	// SpriteMesh is a quad of four Vertex2D vertices.
	// Intended vertex order in the array: 0=TopLeft, 1=TopRight, 2=BottomRight, 3=BottomLeft.
	//
	// Philosophy:
	// SpriteMesh defines the sprite model in model space.
	struct SpriteMesh
	{
		static constexpr size_t VertexCount = 4;

		SpriteMesh()  = default;

		SpriteMesh(const Vertex2D& topLeft, const Vertex2D& topRight, const Vertex2D& bottomRight, const Vertex2D& bottomLeft):
			Vertices{ topLeft, topRight, bottomRight, bottomLeft }
		{
		}

		Vertex2D Vertices[VertexCount];

		Vertex2D& TopLeft()   { return Vertices[0]; }
		Vertex2D& TopRight()   { return Vertices[1]; }
		Vertex2D& BottomRight()   { return Vertices[2]; }
		Vertex2D& BottomLeft()  { return Vertices[3]; }

		const Vertex2D& TopLeft()     const  { return Vertices[0]; }
		const Vertex2D& TopRight()    const  { return Vertices[1]; }
		const Vertex2D& BottomRight() const  { return Vertices[2]; }
		const Vertex2D& BottomLeft()  const  { return Vertices[3]; }

	};

}
