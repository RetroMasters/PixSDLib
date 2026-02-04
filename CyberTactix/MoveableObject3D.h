#pragma once

#include "PixMath.h"

// MoveableObject is the basis class for an object that can move in space.
namespace pix
{
	class MoveableObject3D
	{
	public:

		MoveableObject3D();

		MoveableObject3D(const Transform3D& transform);

		MoveableObject3D(const Transform3D& transform, const Transform3D& prevTransform);

		MoveableObject3D(const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~MoveableObject3D();


		void UpdatePreviousTransform();


		const Transform3D& GetPreviousTransform() const;


		Transform3D  Transform;

	protected:

		Transform3D  prevTransform_;
	};

}

