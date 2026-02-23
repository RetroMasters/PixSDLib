#include "Sprite3D.h"

namespace pix
{

	    Sprite3D::Sprite3D() : MovableObject3D(),
			Mesh(nullptr)
		{
		}

		Sprite3D::Sprite3D(const SpriteMesh* mesh, const Transform3D& transform) : MovableObject3D(transform),
			Mesh(mesh)
		{
		}

		Sprite3D::Sprite3D(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MovableObject3D(transform, prevTransform),
			Mesh(mesh)
		{
		}

		Sprite3D::Sprite3D(const SpriteMesh* mesh, const Vec3& position, const Vec3f& scale, const Rotation3D& rotation) : MovableObject3D(position, scale, rotation),
			Mesh(mesh)
		{
		}

}

