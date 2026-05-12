#pragma once

#include "MovableObject3D.h"
#include "SpriteMesh.h"

namespace pix
{
	// Sprite3D is a movable 3D object that references a SpriteMesh for rendering. 
    // Mesh is non-owning and may be nullptr.
    //
    // Philosophy:
    // Sprite3D is the minimal optimized renderable 3D entity that can move through space.
    // It is the non-hierarchical optimization of Sprite3DNode.
	class Sprite3D : public MovableObject3D
	{
	public:

		Sprite3D() = default;
		Sprite3D(const SpriteMesh* mesh, const Transform3D& transform);
		Sprite3D(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform);
		explicit Sprite3D(const SpriteMesh* mesh, const Vec3& position = Vec3(0.0, 0.0, 0.0), Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f), const Rotation3D& rotation = Rotation3D());

		~Sprite3D() override = default;

		const SpriteMesh* Mesh = nullptr; 
	};

}

