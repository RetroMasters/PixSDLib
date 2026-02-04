#include "Sprite3DEx.h"
#include "MoveableObject3D.h"
#include "TriangleMesh2D.h"


//Sprite is the basis class for a single-textured object that can move in space
namespace pix
{
	
	    Sprite3DEx::Sprite3DEx() : MoveableObject3D(),
			Mesh(nullptr)
		{
		}

		Sprite3DEx::Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform) : MoveableObject3D(transform),
			Mesh(mesh)
		{
		}

		Sprite3DEx::Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MoveableObject3D(transform, prevTransform),
			Mesh(mesh)
		{
		}

		Sprite3DEx::Sprite3DEx(const TriangleMesh2D* mesh, const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) : MoveableObject3D(position, scale, rotation),
			Mesh(mesh)
		{
		}

}
