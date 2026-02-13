#include "Sprite3DEx.h"

namespace pix
{
	
	    Sprite3DEx::Sprite3DEx() : MovableObject3D(),
			Mesh(nullptr)
		{
		}

		Sprite3DEx::Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform) : MovableObject3D(transform),
			Mesh(mesh)
		{
		}

		Sprite3DEx::Sprite3DEx(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MovableObject3D(transform, prevTransform),
			Mesh(mesh)
		{
		}

		Sprite3DEx::Sprite3DEx(const TriangleMesh2D* mesh, const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) : MovableObject3D(position, scale, rotation),
			Mesh(mesh)
		{
		}

}
