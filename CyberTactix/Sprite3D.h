#pragma once

#include "MoveableObject3D.h"
#include "SpriteMesh.h"


//Sprite is the basis class for a single-textured object that can move in space
namespace pix
{
	class Sprite3D : public MoveableObject3D
	{
	public:

		Sprite3D();

		Sprite3D(const SpriteMesh* mesh, const Transform3D& transform);

		Sprite3D(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3D(const SpriteMesh* mesh, const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 0.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~Sprite3D() = default;

		const SpriteMesh* Mesh; //defines the "sprite-model" in model space
	};

}

