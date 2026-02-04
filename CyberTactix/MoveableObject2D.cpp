#include "MoveableObject2D.h"

//#include "Game.h"
//#include "ErrorMessage.h"


namespace pix
{

	MoveableObject2D::MoveableObject2D() :
		Transform{ {0.0, 0.0 } , {1.0f, 1.0f}, 0.0f },
		prevTransform_{ {0.0, 0.0 } , {1.0f, 1.0f}, 0.0f }
	{
	}

	MoveableObject2D::MoveableObject2D(const Transform2D& transform)  :
		Transform{ transform },
		prevTransform_{ transform }
	{
	}

	MoveableObject2D::MoveableObject2D(const Transform2D& transform, const Transform2D& prevTransform)  :
		Transform{ transform },
		prevTransform_{ prevTransform }
	{
	}

	MoveableObject2D::MoveableObject2D(const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation)  :
		Transform{ position , scale, rotation },
		prevTransform_{ Transform }
	{
	}



	void MoveableObject2D::UpdatePreviousTransform() 
	{
		prevTransform_ = Transform;
	}



	const Transform2D& MoveableObject2D::GetPreviousTransform() const 
	{
		return prevTransform_;
	}

}




