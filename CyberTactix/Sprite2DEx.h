#pragma once

#include "MoveableObject2D.h"
#include "TriangleMesh2D.h"


namespace pix
{
	// Sprite2DEx is a movable 2D object that references a TriangleMesh2D for rendering.
	// Mesh is a non-owning pointer to immutable render data and may be nullptr.
	// 
	// Philosophy:
	// Sprite2DEx is the foundation for a renderable 2D entity that can move in space,
	// using a general TriangleMesh2D instead of a specialized SpriteMesh.
	// It is a specialized optimization of Sprite2DExNode that does not participate in a transform hierarchy.
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

