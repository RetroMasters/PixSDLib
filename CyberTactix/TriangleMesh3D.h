#pragma once

#include <vector>
#include <SDL_pixels.h>
#include "PixMath.h"

namespace pix
{
	// Vertex3D is a 3D vertex storing 3D position, color, UV coordinates, and a 3D normal vector.
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

		Vec3f Position = Vec3f(0.0f, 0.0f, 0.0f);
		SDL_Color Color = { 255, 255, 255, 255 };
		Vec2f UV = Vec2f(0.0f, 0.0f);
		Vec3f Normal = Vec3f(0.0f, 0.0f, 1.0f);
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
			return Vertices.size() >= 3 && Vertices.size() % 3 == 0;
		}

		int GetTriangleCount() const
		{
			return Vertices.size() / 3;
		}
	
		std::vector<Vertex3D> Vertices;
	};

}
