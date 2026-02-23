#pragma once

#include "MoveableObject2D.h"
#include "SpriteMesh.h"

namespace pix
{
	// Sprite2D is a movable 2D object that references a SpriteMesh for rendering.
	// Mesh is non-owning and may be nullptr.
	// 
	// Philosophy:
	// Sprite2D is the minimal and most optimized renderable 2D entity that can MOVE in space. 
	// It is the non-hierarchical optimization of Sprite2DNode.
	class Sprite2D : public MovableObject2D
	{
	public:

		Sprite2D();
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform);
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform);
		Sprite2D(const SpriteMesh* mesh, const Vec2& position = { 0.0, 0.0 }, const Vec2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D());

		virtual ~Sprite2D()  = default;

		const SpriteMesh* Mesh; 
	};

}
