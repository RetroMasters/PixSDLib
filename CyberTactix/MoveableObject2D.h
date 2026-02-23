#pragma once

#include "PixMath.h"

namespace pix
{
	// MoveableObject2D is the base class for a spatial 2D object. It represents only a transform (position, scale, rotation).
    // 
    // Philosophy:
    // MoveableObject2D is the foundation for 2D objects that can MOVE in space.
    // It provides native support for visual motion interpolation by maintaining the previous transform state.
	class MovableObject2D
	{
	public:

		MovableObject2D();
		MovableObject2D(const Transform2D& transform);
		MovableObject2D(const Transform2D& transform, const Transform2D& prevTransform);
		MovableObject2D(const Vec2& position, const Vec2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D());

		virtual ~MovableObject2D() = default;

		Transform2D  Transform;

		// Syncs previous transform state with the current one.
		// Call once per update tick before modifying Transform. 
		// This snapshots the current transform into prevTransform_ for interpolation.
		void BeginUpdate();

		const Transform2D& GetPreviousTransform() const;

	protected:

		Transform2D  prevTransform_;
	};

}

