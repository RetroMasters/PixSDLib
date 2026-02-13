#pragma once

#include "PixMath.h"

namespace pix
{
	// MoveableObject3D is the base class for a spatial 3D object. It represents only a transform (position, scale, rotation).
	// 
	// Philosophy:
	// MoveableObject3D is the foundation for 3D objects that can MOVE in space.
	// It provides native support for visual motion interpolation by maintaining the previous transform state.
	class MovableObject3D
	{
	public:

		MovableObject3D();

		MovableObject3D(const Transform3D& transform);

		MovableObject3D(const Transform3D& transform, const Transform3D& prevTransform);

		MovableObject3D(const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~MovableObject3D() = default;

		Transform3D  Transform;

		// Syncs previous transform state with the current one.
		// Call once per update tick before modifying Transform. 
		// This snapshots the current transform into prevTransform_ for interpolation.
		void BeginUpdate();

		const Transform3D& GetPreviousTransform() const;

	protected:

		Transform3D  prevTransform_;
	};

}

