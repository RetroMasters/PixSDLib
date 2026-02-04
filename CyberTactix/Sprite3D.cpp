#include "Sprite3D.h"
#include "MoveableObject3D.h"
#include "SpriteMesh.h"


//Sprite is the basis class for a single-textured object that can move in space
namespace pix
{

	    Sprite3D::Sprite3D() : MoveableObject3D(),
			Mesh(nullptr)
		{
		}

		Sprite3D::Sprite3D(const SpriteMesh* mesh, const Transform3D& transform) : MoveableObject3D(transform),
			Mesh{ mesh }
		{
		}

		Sprite3D::Sprite3D(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MoveableObject3D(transform, prevTransform),
			Mesh(mesh)
		{
		}

		Sprite3D::Sprite3D(const SpriteMesh* mesh, const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) : MoveableObject3D(position, scale, rotation),
			Mesh(mesh)
		{
		}

}

