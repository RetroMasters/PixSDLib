#pragma once

#include<SDL.h>
#include <vector>
#include "pixMath.h"

namespace pix
{
	// Vertex2DEx is a 2D vertex that stores a 2D position, Color, UV-coordinates, and a 3D normal vector.
	// 
	// Philosophy:
	// Vertex2DEx instances are points with attributes that define the TriangleMesh2D. 
	// The normal vector can provide the illusion of a 3D surface for various effects.
	struct Vertex2DEx
	{
		Vertex2DEx()  = default;

		Vertex2DEx(Vector2f position, SDL_Color color, Vector2f uv, Vector3f normal):
			Position(position),
			Color(color),
			UV(uv),
			Normal(normal)
		{
		}

		Vector2f  Position;
		SDL_Color Color;
		Vector2f  UV;
		Vector3f  Normal;
	};


	// TriangleMesh2D is an arbitrary sequence of triangles, each composed of three consecutive Vertex2DEx vertices.
	//
	// Philosophy:
	// TriangleMesh2D defines the sprite model in model space.
	struct TriangleMesh2D
	{
		TriangleMesh2D() = default;

		TriangleMesh2D(const std::vector<Vertex2DEx>& vertices) :
			Vertices(vertices)
		{
		}

		std::vector<Vertex2DEx> Vertices;
	};

}
