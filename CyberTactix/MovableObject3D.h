#pragma once

#include "PixMath.h"

namespace pix
{
	// MovableObject3D is the base class for a spatial 3D object. It represents only a transform: position, scale, rotation.
	// 
	// Philosophy:
	// MovableObject3D is the foundation for 3D objects that move through space.
	// It provides native support for visual motion interpolation by maintaining the previous transform state.
	class MovableObject3D
	{
	public:

		MovableObject3D() = default;

		MovableObject3D(const Transform3D& transform);

		MovableObject3D(const Transform3D& transform, const Transform3D& prevTransform);

		MovableObject3D(const Vec3& position, Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f), const Rotation3D& rotation = Rotation3D());

		virtual ~MovableObject3D() = default;

		// Syncs previous transform state with the current one.
		// Call once per update tick before modifying Transform. 
		// This snapshots the current transform into prevTransform_ for interpolation.
		void BeginUpdate();

		const Transform3D& GetPrevTransform() const;

		Transform3D Transform;

	protected:

		Transform3D prevTransform_;
	};

}

