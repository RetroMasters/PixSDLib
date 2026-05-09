#pragma once

#include "MovableObject2D.h"
#include "SpriteMesh.h"

namespace pix
{
	// Sprite2D is a movable 2D object that references a SpriteMesh for rendering.
    // Mesh is non-owning and may be nullptr.
    //
    // Philosophy:
    // Sprite2D is the minimal optimized renderable 2D entity that can move through space.
    // It is the non-hierarchical optimization of Sprite2DNode.
	class Sprite2D : public MovableObject2D
	{
	public:

		Sprite2D() = default;
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform);
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform);
		Sprite2D(const SpriteMesh* mesh, Vec2 position = Vec2(0.0, 0.0), Vec2f scale = Vec2f(1.0f, 1.0f), Rotation2D rotation = Rotation2D());

	    ~Sprite2D() override = default;

		const SpriteMesh* Mesh = nullptr; 
	};

}
