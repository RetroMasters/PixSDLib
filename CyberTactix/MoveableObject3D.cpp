#include "MoveableObject3D.h"

// MoveableObject is the basis class for an object that can move in space.
namespace pix
{
	
		MovableObject3D::MovableObject3D() :
			Transform(),
			prevTransform_()
		{
		}

		MovableObject3D::MovableObject3D(const Transform3D& transform) :
			Transform(transform),
			prevTransform_(Transform)
		{
		}

		MovableObject3D::MovableObject3D(const Transform3D& transform, const Transform3D& prevTransform) :
			Transform(transform),
			prevTransform_(prevTransform)
		{
		}

		MovableObject3D::MovableObject3D(const Vec3& position, const Vec3f& scale, const Rotation3D& rotation) :
			Transform(position, scale, rotation),
			prevTransform_(Transform)
		{
		}

		void MovableObject3D::BeginUpdate()
		{
			prevTransform_ = Transform;
		}


		const Transform3D& MovableObject3D::GetPreviousTransform() const
		{
			return prevTransform_;
		}

}

