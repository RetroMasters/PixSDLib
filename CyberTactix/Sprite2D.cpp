#include "Sprite2D.h"


namespace pix
{

	Sprite2D::Sprite2D()  : MovableObject2D(),
		Mesh(nullptr)
	{
	}

	Sprite2D::Sprite2D(const SpriteMesh* mesh, const Transform2D& transform)  : MovableObject2D(transform),
		Mesh(mesh)
	{
	}

	Sprite2D::Sprite2D(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform)  : MovableObject2D(transform, prevTransform),
		Mesh(mesh)
	{
	}

	Sprite2D::Sprite2D(const SpriteMesh* mesh, const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation)  : MovableObject2D(position, scale, rotation),
		Mesh(mesh)
	{
	}

}