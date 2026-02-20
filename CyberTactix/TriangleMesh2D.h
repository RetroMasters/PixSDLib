#pragma once

#include<SDL_pixels.h>
#include <vector>
#include "PixMath.h"

namespace pix
{
	// Vertex2DEx is a 2D vertex storing 2D position, Color, UV coordinates, and a 3D normal vector.
	// 
	// Philosophy:
	// Vertex2DEx defines a point with rendering attributes used by TriangleMesh2D.
	// The normal vector can provide the illusion of a 3D surface for lighting effects, or carry any other information.
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

	// TriangleMesh2D stores vertices for a 2D mesh composed of triangles.
    // Each triangle is three consecutive Vertex2DEx entries.
    //
    // Philosophy:
	// TriangleMesh2D defines a 2D model in model space.
	struct TriangleMesh2D
	{
		TriangleMesh2D() = default;

		TriangleMesh2D(const std::vector<Vertex2DEx>& vertices):
			Vertices(vertices)
		{
		}

		bool IsValid() const 
		{ 
			return (Vertices.size() % 3) == 0; 
		}

		size_t GetTriangleCount() const 
		{ 
			return Vertices.size() / 3; 
		}

		std::vector<Vertex2DEx> Vertices;
	};

}
