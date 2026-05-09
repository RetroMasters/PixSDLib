#pragma once

#include "PixMath.h"

namespace pix
{
	// MovableObject2D is the base class for a spatial 2D object. It represents only a transform: position, scale, rotation.
    // 
    // Philosophy:
    // MovableObject2D is the foundation for 2D objects that move through space.
    // It provides native support for visual motion interpolation by maintaining the previous transform state.
	class MovableObject2D
	{
	public:

		MovableObject2D() = default;
		MovableObject2D(const Transform2D& transform);
		MovableObject2D(const Transform2D& transform, const Transform2D& prevTransform);
		MovableObject2D(Vec2 position, Vec2f scale = Vec2f(1.0f, 1.0f), Rotation2D rotation = Rotation2D());

		virtual ~MovableObject2D() = default;

		// Syncs previous transform state with the current one.
		// Call once per update tick before modifying Transform. 
		// This snapshots the current transform into prevTransform_ for interpolation.
		void BeginUpdate();

		const Transform2D& GetPrevTransform() const;

		Transform2D Transform;

	protected:

		Transform2D prevTransform_;
	};

}

