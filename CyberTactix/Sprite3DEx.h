#pragma once

#include "MoveableObject3D.h"
#include "TriangleMesh2D.h"


//Sprite is the basis class for a single-textured object that can move in space
namespace pix
{
	class Sprite3DEx : public MoveableObject3D
	{
	public:

		Sprite3DEx();

		Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform);

		Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3DEx(const TriangleMesh2D* mesh, const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~Sprite3DEx() = default;

		const TriangleMesh2D* Mesh; //defines the "sprite-model" in model space
	};

}
