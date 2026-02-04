#include "Sprite2DEx.h"

//#include "Game.h"
//#include "ErrorMessage.h"

namespace pix
{

	Sprite2DEx::Sprite2DEx()  : MoveableObject2D(),
		Mesh(nullptr)
	{
	}

	Sprite2DEx::Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform)  : MoveableObject2D(transform),
		Mesh(mesh)
	{
	}

	Sprite2DEx::Sprite2DEx(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform)  : MoveableObject2D(transform, prevTransform),
		Mesh(mesh)
	{
	}

	Sprite2DEx::Sprite2DEx(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation)  : MoveableObject2D(position, scale, rotation),
		Mesh(mesh)
	{
	}

}





