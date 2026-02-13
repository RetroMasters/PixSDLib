#pragma once

#include "MoveableObject3D.h"
#include "SpriteMesh.h"


// Sprite3D is a movable 3D object that references a SpriteMesh for rendering.
// Mesh is non-owning and may be nullptr.
// 
// Philosophy:	
// Sprite3D is the minimal and most optimized renderable 3D entity that can MOVE in space. 
// It is the non-hierarchical optimization of Sprite3DNode.
namespace pix
{
	class Sprite3D : public MovableObject3D
	{
	public:

		Sprite3D();

		Sprite3D(const SpriteMesh* mesh, const Transform3D& transform);

		Sprite3D(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3D(const SpriteMesh* mesh, const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~Sprite3D() = default;

		const SpriteMesh* Mesh; 
	};

}

