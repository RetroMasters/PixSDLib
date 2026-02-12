#pragma once

#include "MoveableObject2D.h"
#include "SpriteMesh.h"

namespace pix
{
	// Sprite2D is a movable 2D object that references a SpriteMesh for rendering.
	// Mesh is a non-owning pointer to immutable render data and may be nullptr.
	// 
	// Philosophy:
	// Sprite2D is the minimal and most optimized renderable 2D entity that can move in space. 
	// It is a specialized optimization of Sprite2DNode that does not participate in a transform hierarchy.
	class Sprite2D : public MovableObject2D
	{
	public:

		Sprite2D();
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform);
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform);
		Sprite2D(const SpriteMesh* mesh, const Vector2d& position = { 0.0, 0.0 }, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D());

		virtual ~Sprite2D()  = default;

		const SpriteMesh* Mesh; 
	};

}
