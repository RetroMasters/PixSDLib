#pragma once

#include "MovableObject2D.h"
#include "TriangleMesh2D.h"

namespace pix
{
	// Sprite2DEx is a movable 2D object that references a TriangleMesh2D for rendering.
	// Mesh is non-owning and may be nullptr.
	// 
	// Philosophy:
	// Sprite2DEx is a minimal renderable 2D entity that can move through space, using a general TriangleMesh2D instead of SpriteMesh.
    // It is the non-hierarchical optimization of Sprite2DExNode.
	class Sprite2DEx : public MovableObject2D
	{
	public:

		Sprite2DEx() = default;
		Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform);
		Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform);
		explicit Sprite2DEx(const TriangleMesh2D* mesh, Vec2 position = Vec2(0.0, 0.0), Vec2f scale = Vec2f(1.0f, 1.0f), Rotation2D rotation = Rotation2D());

		~Sprite2DEx() override = default;

		const TriangleMesh2D* Mesh = nullptr; 

	};

}

