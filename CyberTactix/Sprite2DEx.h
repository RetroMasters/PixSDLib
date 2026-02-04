#pragma once

#include "MoveableObject2D.h"
#include "TriangleMesh2D.h"

// SpriteEx is the basis class for a mesh-based object that can move in space.

namespace pix
{
	class Sprite2DEx : public MoveableObject2D
	{
	public:

		Sprite2DEx() ;
		Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform) ;
		Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform) ;
		Sprite2DEx(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D()) ;

		virtual ~Sprite2DEx()  = default;

		const TriangleMesh2D* Mesh; //defines the "sprite-model" in model space

	};

}

