#include "MovableObject2D.h"

namespace pix
{

	MovableObject2D::MovableObject2D(const Transform2D& transform) :
		Transform(transform),
		prevTransform_(transform)
	{
	}

	MovableObject2D::MovableObject2D(const Transform2D& transform, const Transform2D& prevTransform) :
		Transform(transform),
		prevTransform_(prevTransform)
	{
	}

	MovableObject2D::MovableObject2D(Vec2 position, Vec2f scale, Rotation2D rotation) :
		Transform(position, scale, rotation),
		prevTransform_(Transform)
	{
	}

	void MovableObject2D::BeginUpdate() 
	{
		prevTransform_ = Transform;
	}

	const Transform2D& MovableObject2D::GetPrevTransform() const 
	{
		return prevTransform_;
	}

}




