#pragma once

#include "PixMath.h"


// MoveableObject is the basis class for an object that can move in space.
namespace pix
{
	class MoveableObject2D
	{
	public:

		MoveableObject2D();
		MoveableObject2D(const Transform2D& transform);
		MoveableObject2D(const Transform2D& transform, const Transform2D& prevTransform);
		MoveableObject2D(const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D());

		virtual ~MoveableObject2D() = default;

		void UpdatePreviousTransform();

		const Transform2D& GetPreviousTransform() const;



		Transform2D  Transform;

	protected:

		Transform2D  prevTransform_;
	};

}

