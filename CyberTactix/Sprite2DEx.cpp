#include "Sprite2DEx.h"

namespace pix
{

	Sprite2DEx::Sprite2DEx()  : MovableObject2D(),
		Mesh(nullptr)
	{
	}

	Sprite2DEx::Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform)  : MovableObject2D(transform),
		Mesh(mesh)
	{
	}

	Sprite2DEx::Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform)  : MovableObject2D(transform, prevTransform),
		Mesh(mesh)
	{
	}

	Sprite2DEx::Sprite2DEx(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation)  : MovableObject2D(position, scale, rotation),
		Mesh(mesh)
	{
	}

}





