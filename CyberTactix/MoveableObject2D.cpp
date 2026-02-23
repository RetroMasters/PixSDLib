#include "MoveableObject2D.h"

namespace pix
{

	MovableObject2D::MovableObject2D() :
		Transform{ {0.0, 0.0 } , {1.0f, 1.0f}, 0.0f },
		prevTransform_{ Transform }
	{
	}

	MovableObject2D::MovableObject2D(const Transform2D& transform) :
		Transform{ transform },
		prevTransform_{ transform }
	{
	}

	MovableObject2D::MovableObject2D(const Transform2D& transform, const Transform2D& prevTransform) :
		Transform{transform},
		prevTransform_{ prevTransform }
	{
	}

	MovableObject2D::MovableObject2D(const Vec2& position, const Vec2f& scale, const Rotation2D& rotation)  :
		Transform{ position , scale, rotation },
		prevTransform_{ Transform }
	{
	}

	void MovableObject2D::BeginUpdate() 
	{
		prevTransform_ = Transform;
	}

	const Transform2D& MovableObject2D::GetPreviousTransform() const 
	{
		return prevTransform_;
	}

}




