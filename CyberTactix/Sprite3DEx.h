#pragma once

#include "MoveableObject3D.h"
#include "TriangleMesh2D.h"

namespace pix
{
	// Sprite3DEx is a movable 3D object that references a TriangleMesh2D for flat geometry rendering in 3D.
	// Mesh is non-owning and may be nullptr.
	// 
	// Philosophy:
	// Sprite3DEx is a minimal renderable 3D entity that can move in space, using a general TriangleMesh2D instead of SpriteMesh.
	// It is the non-hierarchical optimization of Sprite3DExNode.
	class Sprite3DEx : public MovableObject3D
	{
	public:

		Sprite3DEx();

		Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform);

		Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3DEx(const TriangleMesh2D* mesh, const Vec3& position, const Vec3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~Sprite3DEx() = default;

		const TriangleMesh2D* Mesh; 
	};

}
