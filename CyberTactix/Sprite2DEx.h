#pragma once

#include "MoveableObject2D.h"
#include "TriangleMesh2D.h"


namespace pix
{
	// Sprite2DEx is a movable 2D object that references a TriangleMesh2D for rendering.
	// Mesh is non-owning and may be nullptr.
	// 
	// Philosophy:
	// Sprite2DEx is a minimal renderable 2D entity that can move in space, using a general TriangleMesh2D instead of SpriteMesh.
    // It is the non-hierarchical optimization of Sprite2DExNode.
	class Sprite2DEx : public MovableObject2D
	{
	public:

		Sprite2DEx() ;
		Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform) ;
		Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform) ;
		Sprite2DEx(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D()) ;

		virtual ~Sprite2DEx()  = default;

		const TriangleMesh2D* Mesh; 

	};

}

