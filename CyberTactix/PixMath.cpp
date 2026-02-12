
#include "PixMath.h"

namespace pix
{

	Rotation2D::Rotation2D(): xAxis_(1.0f, 0.0f) {}

	Rotation2D::Rotation2D(float degrees) 
	{
		Set(degrees);
	}

	Rotation2D::Rotation2D(float x, float y): xAxis_(x, y) {}

	void Rotation2D::SetToIdentity()
	{
		xAxis_ = { 1.0f, 0.0f };
	}

	void Rotation2D::Set(float degrees) 
	{
		degrees *= RADIANS_PER_DEGREE_F;

		xAxis_ = { std::cosf(degrees), std::sinf(degrees) }; // No need for normalization as numerical errors don't accumulate
	}

	const Vector2f& Rotation2D::GetXAxis() const 
	{
		return xAxis_;
	}

	Vector2f Rotation2D::GetYAxis() const 
	{
		return Vector2f(-xAxis_.Y, xAxis_.X);
	}

	float Rotation2D::GetAngle() const
	{
		return std::atan2(xAxis_.Y, xAxis_.X) * DEGREES_PER_RADIAN_F;
	}

	Rotation2D& Rotation2D::Inverse() 
	{
		xAxis_ = { xAxis_.X, -xAxis_.Y };
		return *this;
	}

	Rotation2D Rotation2D::GetInverse() const
	{
		return Rotation2D(xAxis_.X, -xAxis_.Y);
	}

	Rotation2D& Rotation2D::AddRotation(float deltaDegrees) 
	{
		deltaDegrees *= RADIANS_PER_DEGREE_F;

		Vector2f localXAxis = { std::cosf(deltaDegrees), std::sinf(deltaDegrees) };

		xAxis_ = { xAxis_.X * localXAxis.X - xAxis_.Y * localXAxis.Y,
				   xAxis_.Y * localXAxis.X + xAxis_.X * localXAxis.Y };

		xAxis_.Normalize();
		return *this;
	}

	Rotation2D& Rotation2D::AddRotation(const Rotation2D& deltaRotation) 
	{
		xAxis_ = { xAxis_.X * deltaRotation.xAxis_.X - xAxis_.Y * deltaRotation.xAxis_.Y,
				   xAxis_.Y * deltaRotation.xAxis_.X + xAxis_.X * deltaRotation.xAxis_.Y };

		xAxis_.Normalize();
		return *this;
	}






	Transform2D::Transform2D(): Position(0.0, 0.0), Scale(1.0f, 1.0f), Rotation() {}

	Transform2D::Transform2D(const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation): Position{ position }, Scale{ scale }, Rotation{ rotation } {}


	void Transform2D::ApplyToPoints(Vector2d* points, int count) const
	{
		const Vector2d xAxis(Rotation.GetXAxis()); // Convert to Vector2D for upcoming use
		const Vector2d scale(Scale); // Convert to Vector2d for upcoming use

		for (int i = 0; i < count; i++)
		{
			// Scale the current mesh point:
			Vector2d transformedPoint = points[i] * scale; //Vector2d(Scale);

			// Rotate the current mesh point:
			transformedPoint = RotatePointRaw(xAxis, transformedPoint);

			// Position the transformed mesh point:
			points[i] = Position + transformedPoint;
		}

	}



	void Transform2D::ApplyToPoint(Vector2d& point) const 
	{
		const Vector2d xAxis(Rotation.GetXAxis()); // Convert to Vector2D for use

		// Scale the current mesh point:
		Vector2d transformedPoint = point * Vector2d(Scale);

		// Rotate the current mesh point:
		transformedPoint = RotatePointRaw(xAxis, transformedPoint);

		// Position the transformed mesh point:
		point = Position + transformedPoint;
	}

	void Transform2D::ApplyInverseToPoint(Vector2d& point) 
	{
		// Reverse translation
		point -= Position;

		// Reverse rotation
		point = Rotation.GetInverse().RotatePoint(point);

		// Reverse scale
		point.X = DivideSafe(point.X, (double)Scale.X);
		point.Y = DivideSafe(point.Y, (double)Scale.Y);
	}




	//############################################################################### 3D TYPES ################################################################

	Rotation3D::Rotation3D(): xAxis_(1.0f, 0.0f, 0.0f), yAxis_(0.0f, 1.0f, 0.0f) {}

	Rotation3D::Rotation3D(const Vector3f& rotationX, const Vector3f& rotationY) :xAxis_(rotationX), yAxis_(rotationY)
	{
		Normalize();
	}

	void Rotation3D::SetToIdentity() 
	{
		xAxis_ = { 1.0f, 0.0f, 0.0f };
		yAxis_ = { 0.0f, 1.0f, 0.0f };
	}

	const Vector3f& Rotation3D::GetXAxis() const
	{
		return xAxis_;
	}

	const Vector3f& Rotation3D::GetYAxis() const 
	{
		return yAxis_;
	}

	Vector3f Rotation3D::GetZAxis() const
	{
		return yAxis_.CrossProduct(xAxis_);
	}

	Rotation3D& Rotation3D::Inverse()// The projection of the global coordinate system on the member rotation
	{
		const Vector3f zAxis = GetZAxis();
		const Vector3f temp(xAxis_.X, yAxis_.X, zAxis.X);

		yAxis_ = { xAxis_.Y, yAxis_.Y, zAxis.Y };
		xAxis_ = temp;

		return Normalize();
	}

	Rotation3D Rotation3D::GetInverse() const // The projection of the global coordinate system on the member rotation
	{
		const Vector3f zAxis = GetZAxis();

		return Rotation3D(Vector3f(xAxis_.X, yAxis_.X, zAxis.X), Vector3f(xAxis_.Y, yAxis_.Y, zAxis.Y));
	}

	Rotation3D Rotation3D::ToLocalRotation(const Rotation3D& globalRotation) const 
	{
		const Vector3f zAxis = GetZAxis();

		const Vector3f rotationX = { xAxis_.DotProduct(globalRotation.GetXAxis()),
							   yAxis_.DotProduct(globalRotation.GetXAxis()),
							   zAxis.DotProduct(globalRotation.GetXAxis()) };

		const Vector3f rotationY = { xAxis_.DotProduct(globalRotation.GetYAxis()),
							   yAxis_.DotProduct(globalRotation.GetYAxis()),
							   zAxis.DotProduct(globalRotation.GetYAxis()) };

		return Rotation3D(rotationX, rotationY);
	}

	Rotation3D& Rotation3D::AddGlobalRotation(const Rotation3D& globalRotation) // Treat member rotation as two points to rotate
	{
		const Vector3f globalRotationZAxis = globalRotation.GetZAxis();

		xAxis_ = globalRotation.xAxis_ * xAxis_.X + globalRotation.yAxis_ * xAxis_.Y + globalRotationZAxis * xAxis_.Z;
		yAxis_ = globalRotation.xAxis_ * yAxis_.X + globalRotation.yAxis_ * yAxis_.Y + globalRotationZAxis * yAxis_.Z;

		return Normalize();
	}

	Rotation3D& Rotation3D::AddLocalRotation(const Rotation3D& localRotation) // Treat localRotation as two rotated points
	{
		const Vector3f zAxis = GetZAxis();
		const Vector3f temp = xAxis_ * localRotation.xAxis_.X + yAxis_ * localRotation.xAxis_.Y + zAxis * localRotation.xAxis_.Z;

		yAxis_ = xAxis_ * localRotation.yAxis_.X + yAxis_ * localRotation.yAxis_.Y + zAxis * localRotation.yAxis_.Z;
		xAxis_ = temp;

		return Normalize();
	}

	Rotation3D& Rotation3D::AddGlobalRotation(Vector3f rotAxis, float degrees) 
	{
		rotAxis.Normalize();

		const float squareLength = rotAxis.GetSquareLength();

		if (squareLength < 0.1f || squareLength > 10.0f) return *this; // The threshold is arbitrary but sufficient to know that the rotAxis must be a bad input

		//if (rotAxis.X == 0.0f && rotAxis.Y == 0.0f && rotAxis.Z == 0.0f) return *this;

		if (std::abs(rotAxis.Y) < std::abs(rotAxis.X) + std::abs(rotAxis.Z))
		{
			const Rotation3D rotation = Rotation3D(rotAxis, Vector3f(0.0f, 1.0f, 0.0f).Normalize());
			const Rotation3D inversedRotation = rotation.GetInverse();

			return AddGlobalRotation(inversedRotation).AddGlobalRotationX(degrees).AddGlobalRotation(rotation);
		}
		else
		{
			const Rotation3D rotation = Rotation3D(rotAxis, Vector3f(1.0f, 0.0f, 0.0f).Normalize());
			const Rotation3D inversedRotation = rotation.GetInverse();

			return AddGlobalRotation(inversedRotation).AddGlobalRotationX(degrees).AddGlobalRotation(rotation);
		}
	}

	Rotation3D& Rotation3D::AddGlobalRotationX(float degrees)  // Treat like a 2D-rotation
	{
		const float rad = degrees * RADIANS_PER_DEGREE_F;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		xAxis_ = { xAxis_.X, -xAxis_.Z * s + xAxis_.Y * c, xAxis_.Z * c + xAxis_.Y * s };
		yAxis_ = { yAxis_.X, -yAxis_.Z * s + yAxis_.Y * c, yAxis_.Z * c + yAxis_.Y * s };

		return Normalize();
	}

	Rotation3D& Rotation3D::AddGlobalRotationY(float degrees)// Treat like a 2D-rotation
	{
		const float rad = degrees * RADIANS_PER_DEGREE_F;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		xAxis_ = { xAxis_.X * c + xAxis_.Z * s, xAxis_.Y, -xAxis_.X * s + xAxis_.Z * c };
		yAxis_ = { yAxis_.X * c + yAxis_.Z * s, yAxis_.Y, -yAxis_.X * s + yAxis_.Z * c };

		return Normalize();
	}

	Rotation3D& Rotation3D::AddGlobalRotationZ(float degrees)// Treat like a 2D-rotation
	{
		const float rad = degrees * RADIANS_PER_DEGREE_F;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		xAxis_ = { xAxis_.X * c - xAxis_.Y * s, xAxis_.X * s + xAxis_.Y * c, xAxis_.Z };
		yAxis_ = { yAxis_.X * c - yAxis_.Y * s, yAxis_.X * s + yAxis_.Y * c, yAxis_.Z };

		return Normalize();
	}	

	Rotation3D& Rotation3D::AddLocalRotationX(float degrees)// Treat localRotation as a point to rotate
	{
		const float rad = degrees * RADIANS_PER_DEGREE_F;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		const Vector3f zAxis = yAxis_.CrossProduct(xAxis_);
		//yAxis_ = yAxis_ * s + z * (-c);  // localRotatX.Z = -c,  localRotX.Y = s
		yAxis_ = yAxis_ * c + zAxis * s;

		return Normalize();
	}

	Rotation3D& Rotation3D::AddLocalRotationY(float degrees) // Treat localRotation as a point to rotate
	{
		const float rad = degrees * RADIANS_PER_DEGREE_F;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		const Vector3f zAxis = yAxis_.CrossProduct(xAxis_);
		xAxis_ = xAxis_ * c + zAxis * (-s); // localRotX.X = c, localRotX.Z = -s

		return Normalize();
	}

	Rotation3D& Rotation3D::AddLocalRotationZ(float degrees)// Treat localRotation as two points to rotate
	{
		const float rad = degrees * RADIANS_PER_DEGREE_F;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		const Vector3f tempX = xAxis_;
		xAxis_ = xAxis_ * c + yAxis_ * s; // localRotX.X = c, localRotX.Y = s						   
		yAxis_ = tempX * (-s) + yAxis_ * c; // localRotY.X = -s, localRotY.Y = c;

		return Normalize();
	}

	Vector3f Rotation3D::RotatePoint(const Vector3f& point) const 
	{
		return (xAxis_ * point.X) + (yAxis_ * point.Y) + (GetZAxis() * point.Z);
	}

	Vector3d Rotation3D::RotatePoint(const Vector3d& point) const 
	{
		// Convert to Vector3d
		Vector3d xAxis(xAxis_);
		Vector3d yAxis(yAxis_);
		Vector3d axisZ(GetZAxis());

		return (xAxis * point.X) + (yAxis * point.Y) + (axisZ * point.Z);
	}

	Rotation3D& Rotation3D::Normalize() 
	{
		xAxis_.Normalize();
		const float overlapOnX = yAxis_.DotProduct(xAxis_);
		yAxis_ -= (xAxis_ * overlapOnX);
		yAxis_.Normalize();

		return *this;
	}






	Transform3D::Transform3D(): Position(0.0, 0.0, 0.0), Scale(1.0, 1.0, 1.0), Rotation() {}

	Transform3D::Transform3D(const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) :Position(position), Scale(scale), Rotation(rotation) {}

	void Transform3D::ApplyToPoints(Vector3d* points, int count) const 
	{
		// Convert to Vector3d for upcoming use:
		const Vector3d scale(Scale);
		const Vector3d xAxis(Rotation.GetXAxis());
		const Vector3d yAxis(Rotation.GetYAxis());
		const Vector3d zAxis(Rotation.GetZAxis());

		for (int i = 0; i < count; i++)
		{
			// Scale the mesh point:
			points[i] = points[i] * scale;

			// Rotate the mesh point:
			points[i] = xAxis * points[i].X + yAxis * points[i].Y + zAxis * points[i].Z;

			// Position the mesh point:
			points[i] += Position;
		}
	}

	void Transform3D::ApplyToPoint(Vector3d& point) const 
	{
		// Convert to Vector3d for upcoming use:
		const Vector3d scale(Scale);
		const Vector3d xAxis(Rotation.GetXAxis());
		const Vector3d yAxis(Rotation.GetYAxis());
		const Vector3d zAxis(Rotation.GetZAxis());

		// Scale the point:
		point *= scale;

		// Rotate the point:
		point = xAxis * point.X + yAxis * point.Y + zAxis * point.Z;

		// Position the point:
		point += Position;
	}

}