#include "MovableObject3D.h"

namespace pix
{
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

		MovableObject3D::MovableObject3D(const Vec3& position, Vec3f scale, const Rotation3D& rotation) :
			Transform(position, scale, rotation),
			prevTransform_(Transform)
		{
		}

		void MovableObject3D::BeginUpdate()
		{
			prevTransform_ = Transform;
		}


		const Transform3D& MovableObject3D::GetPrevTransform() const
		{
			return prevTransform_;
		}

}

