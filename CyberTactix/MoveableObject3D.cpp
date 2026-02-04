#include "MoveableObject3D.h"
#include "PixMath.h"

// MoveableObject is the basis class for an object that can move in space.
namespace pix
{
	
		MoveableObject3D::MoveableObject3D() :
			Transform{ { 0.0, 0.0, 0.0 } },
			prevTransform_{ { 0.0, 0.0, 0.0 } }
		{
		}

		MoveableObject3D::MoveableObject3D(const Transform3D& transform) :
			Transform(transform),
			prevTransform_(transform)
		{
		}

		MoveableObject3D::MoveableObject3D(const Transform3D& transform, const Transform3D& prevTransform) :
			Transform(transform),
			prevTransform_(prevTransform)
		{
		}

		MoveableObject3D::MoveableObject3D(const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) :
			Transform(position, scale, rotation),
			prevTransform_(Transform)
		{
		}

		MoveableObject3D::~MoveableObject3D() {}


		void MoveableObject3D::UpdatePreviousTransform()
		{
			prevTransform_ = Transform;
		}


		const Transform3D& MoveableObject3D::GetPreviousTransform() const
		{
			return prevTransform_;
		}

}

