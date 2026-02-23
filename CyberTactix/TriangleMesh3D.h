#pragma once

#include<SDL_pixels.h>
#include <vector>
#include "PixMath.h"

namespace pix
{
	// Vertex3D is a 3D vertex storing 3D position, Color, UV coordinates, and a 3D normal vector.
    // 
    // Philosophy:
    // Vertex3D defines a point with rendering attributes used by TriangleMesh3D.
    // The normal vector can provide the illusion of a 3D surface for lighting effects, or carry any other information.
	struct Vertex3D
	{
		Vertex3D() = default;

		Vertex3D(Vec3f position, SDL_Color color, Vec2f uv, Vec3f normal):
			Position(position),
			Color(color),
			UV(uv),
			Normal(normal)
		{
		}

		Vec3f   Position;
		SDL_Color  Color;
		Vec2f   UV;
		Vec3f   Normal;
	};


	// TriangleMesh3D stores vertices for a 3D mesh composed of triangles.
	// Each triangle is three consecutive Vertex3D entries.
	//
	// Philosophy:
	// TriangleMesh3D defines a 3D model in model space.
	struct TriangleMesh3D
	{
		TriangleMesh3D() = default;
	 
		TriangleMesh3D(const std::vector<Vertex3D>& vertices):
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
	
		std::vector<Vertex3D> Vertices;
	};

}
