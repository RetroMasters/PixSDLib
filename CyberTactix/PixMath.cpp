
#include "PixMath.h"

namespace pix
{

	Rotation2D::Rotation2D(): xAxis_(1.0f, 0.0f) {}

	Rotation2D::Rotation2D(float degrees) 
	{
		Set(degrees);
	}

	void Rotation2D::Set(float degrees)
	{
		degrees *= (float)RADIANS_PER_DEGREE;

		// No need for normalization as numerical errors don't accumulate
		xAxis_ = { std::cosf(degrees), std::sinf(degrees) };
	}

	Rotation2D& Rotation2D::AddRotation(float deltaDegrees)
	{
		deltaDegrees *= (float)RADIANS_PER_DEGREE;

		Vec2f localXAxis = { std::cosf(deltaDegrees), std::sinf(deltaDegrees) };

		xAxis_ = { xAxis_.X * localXAxis.X - xAxis_.Y * localXAxis.Y,
				   xAxis_.Y * localXAxis.X + xAxis_.X * localXAxis.Y };

		xAxis_.Normalize();
		return *this;
	}

	Rotation2D& Rotation2D::AddRotation(Rotation2D deltaRotation)
	{
		xAxis_ = { xAxis_.X * deltaRotation.xAxis_.X - xAxis_.Y * deltaRotation.xAxis_.Y,
				   xAxis_.Y * deltaRotation.xAxis_.X + xAxis_.X * deltaRotation.xAxis_.Y };

		xAxis_.Normalize();
		return *this;
	}

	void Rotation2D::SetToIdentity()
	{
		xAxis_ = { 1.0f, 0.0f };
	}

	Rotation2D& Rotation2D::Inverse()
	{
		xAxis_.Y = -xAxis_.Y;
		return *this;
	}

	void Rotation2D::RotatePoint(Vec2f& point) const
	{
		point = { xAxis_.X * point.X - xAxis_.Y * point.Y,
				  xAxis_.Y * point.X + xAxis_.X * point.Y };
	}

	void Rotation2D::InverseRotatePoint(Vec2f& point) const
	{
		point = { xAxis_.X * point.X + xAxis_.Y * point.Y,
				 -xAxis_.Y * point.X + xAxis_.X * point.Y };
	}

	void Rotation2D::RotatePoint(Vec2& point) const
	{
		Vec2 xAxis(xAxis_); // Convert to Vector2d for use

		point = { xAxis.X * point.X - xAxis.Y * point.Y,
				  xAxis.Y * point.X + xAxis.X * point.Y };
	}

	void Rotation2D::InverseRotatePoint(Vec2& point) const
	{
		Vec2 xAxis(xAxis_); // Convert to Vector2d for use

		point = { xAxis.X * point.X + xAxis.Y * point.Y,
				 -xAxis.Y * point.X + xAxis.X * point.Y };
	}

	Vec2f Rotation2D::GetXAxis() const 
	{
		return xAxis_;
	}

	Vec2f Rotation2D::GetYAxis() const 
	{
		return Vec2f(-xAxis_.Y, xAxis_.X);
	}

	float Rotation2D::GetAngle() const
	{
		return std::atan2(xAxis_.Y, xAxis_.X) * (float)DEGREES_PER_RADIAN;
	}

	Rotation2D Rotation2D::GetInverse() const
	{
		return Rotation2D(xAxis_.X, -xAxis_.Y);
	}

	Rotation2D::Rotation2D(float x, float y) : xAxis_(x, y)
	{
	}


	



	Transform2D::Transform2D(): 
		Position(0.0, 0.0), 
		Scale(1.0f, 1.0f), 
		Rotation() 
	{
	}

	Transform2D::Transform2D(Vec2 position, Vec2f scale, Rotation2D rotation): 
		Position(position), 
		Scale(scale), 
		Rotation(rotation) 
	{
	}


	void Transform2D::TransformPoints(Vec2* points, int count) const
	{
		// Convert Scale to Vec2d for repeated use
		Vec2 scale = (Vec2)Scale;

		for (int i = 0; i < count; i++)
		{
			// Scale the current mesh point
			Vec2 transformedPoint = points[i] * scale; 

			// Rotate the current mesh point
			Rotation.RotatePoint(transformedPoint);

			// Translate the transformed mesh point
			points[i] = transformedPoint + Position;
		}
	}

	void Transform2D::TransformPoint(Vec2& point) const 
	{
		// Scale the current mesh point
		Vec2 transformedPoint = point * (Vec2)Scale;

		// Rotate the current mesh point
		Rotation.RotatePoint(transformedPoint);

		// Translate the transformed mesh point
		point = transformedPoint + Position;
	}

	void Transform2D::InverseTransformPoints(Vec2* points, int count) const
	{
		// Precompute inverse scale. DivideSafe() keeps it finite.
		Vec2 invScale(GetSafeDivision(1.0, (double)Scale.X), GetSafeDivision(1.0, (double)Scale.Y));

		for (int i = 0; i < count; i++)
		{
			// Reverse translation
			points[i] -= Position;

			// Reverse rotation
			Rotation.InverseRotatePoint(points[i]);

			// Reverse scale
			points[i] *= invScale;
		}
	}

	void Transform2D::InverseTransformPoint(Vec2& point) const
	{
		// Reverse translation
		point -= Position;

		// Reverse rotation
		Rotation.InverseRotatePoint(point);

		// Reverse scale
		point.X = GetSafeDivision(point.X, (double)Scale.X);
		point.Y = GetSafeDivision(point.Y, (double)Scale.Y);
	}

	

	//############################################################################### 3D TYPES ################################################################

	Rotation3D::Rotation3D(): xAxis_(1.0f, 0.0f, 0.0f), yAxis_(0.0f, 1.0f, 0.0f) {}

	Rotation3D::Rotation3D(const Vec3f& rotationX, const Vec3f& rotationY) :xAxis_(rotationX), yAxis_(rotationY)
	{
		Normalize();
	}

	void Rotation3D::SetToIdentity() 
	{
		xAxis_ = { 1.0f, 0.0f, 0.0f };
		yAxis_ = { 0.0f, 1.0f, 0.0f };
	}

	const Vec3f& Rotation3D::GetXAxis() const
	{
		return xAxis_;
	}

	const Vec3f& Rotation3D::GetYAxis() const 
	{
		return yAxis_;
	}

	Vec3f Rotation3D::GetZAxis() const
	{
		return yAxis_.GetCrossProduct(xAxis_);
	}

	Rotation3D& Rotation3D::Inverse()// The projection of the global coordinate system on the member rotation
	{
		const Vec3f zAxis = GetZAxis();
		const Vec3f temp(xAxis_.X, yAxis_.X, zAxis.X);

		yAxis_ = { xAxis_.Y, yAxis_.Y, zAxis.Y };
		xAxis_ = temp;

		return Normalize();
	}

	Rotation3D Rotation3D::GetInverse() const // The projection of the global coordinate system on the member rotation
	{
		const Vec3f zAxis = GetZAxis();

		return Rotation3D(Vec3f(xAxis_.X, yAxis_.X, zAxis.X), Vec3f(xAxis_.Y, yAxis_.Y, zAxis.Y));
	}

	Rotation3D Rotation3D::ToLocalRotation(const Rotation3D& globalRotation) const 
	{
		const Vec3f zAxis = GetZAxis();

		const Vec3f rotationX = { xAxis_.GetDotProduct(globalRotation.GetXAxis()),
							   yAxis_.GetDotProduct(globalRotation.GetXAxis()),
							   zAxis.GetDotProduct(globalRotation.GetXAxis()) };

		const Vec3f rotationY = { xAxis_.GetDotProduct(globalRotation.GetYAxis()),
							   yAxis_.GetDotProduct(globalRotation.GetYAxis()),
							   zAxis.GetDotProduct(globalRotation.GetYAxis()) };

		return Rotation3D(rotationX, rotationY);
	}

	Rotation3D& Rotation3D::AddGlobalRotation(const Rotation3D& globalRotation) // Treat member rotation as two points to rotate
	{
		const Vec3f globalRotationZAxis = globalRotation.GetZAxis();

		xAxis_ = globalRotation.xAxis_ * xAxis_.X + globalRotation.yAxis_ * xAxis_.Y + globalRotationZAxis * xAxis_.Z;
		yAxis_ = globalRotation.xAxis_ * yAxis_.X + globalRotation.yAxis_ * yAxis_.Y + globalRotationZAxis * yAxis_.Z;

		return Normalize();
	}

	Rotation3D& Rotation3D::AddLocalRotation(const Rotation3D& localRotation) // Treat localRotation as two rotated points
	{
		const Vec3f zAxis = GetZAxis();
		const Vec3f temp = xAxis_ * localRotation.xAxis_.X + yAxis_ * localRotation.xAxis_.Y + zAxis * localRotation.xAxis_.Z;

		yAxis_ = xAxis_ * localRotation.yAxis_.X + yAxis_ * localRotation.yAxis_.Y + zAxis * localRotation.yAxis_.Z;
		xAxis_ = temp;

		return Normalize();
	}

	Rotation3D& Rotation3D::AddGlobalRotation(Vec3f rotAxis, float degrees) 
	{
		rotAxis.Normalize();

		const float squareLength = rotAxis.GetSquareLength();

		if (squareLength < 0.1f || squareLength > 10.0f) return *this; // The threshold is arbitrary but sufficient to know that the rotAxis must be a bad input

		//if (rotAxis.X == 0.0f && rotAxis.Y == 0.0f && rotAxis.Z == 0.0f) return *this;

		if (std::abs(rotAxis.Y) < std::abs(rotAxis.X) + std::abs(rotAxis.Z))
		{
			const Rotation3D rotation = Rotation3D(rotAxis, Vec3f(0.0f, 1.0f, 0.0f).Normalize());
			const Rotation3D inversedRotation = rotation.GetInverse();

			return AddGlobalRotation(inversedRotation).AddGlobalRotationX(degrees).AddGlobalRotation(rotation);
		}
		else
		{
			const Rotation3D rotation = Rotation3D(rotAxis, Vec3f(1.0f, 0.0f, 0.0f).Normalize());
			const Rotation3D inversedRotation = rotation.GetInverse();

			return AddGlobalRotation(inversedRotation).AddGlobalRotationX(degrees).AddGlobalRotation(rotation);
		}
	}

	Rotation3D& Rotation3D::AddGlobalRotationX(float degrees)  // Treat like a 2D-rotation
	{
		const float rad = degrees * (float)RADIANS_PER_DEGREE;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		xAxis_ = { xAxis_.X, -xAxis_.Z * s + xAxis_.Y * c, xAxis_.Z * c + xAxis_.Y * s };
		yAxis_ = { yAxis_.X, -yAxis_.Z * s + yAxis_.Y * c, yAxis_.Z * c + yAxis_.Y * s };

		return Normalize();
	}

	Rotation3D& Rotation3D::AddGlobalRotationY(float degrees)// Treat like a 2D-rotation
	{
		const float rad = degrees * (float)RADIANS_PER_DEGREE;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		xAxis_ = { xAxis_.X * c + xAxis_.Z * s, xAxis_.Y, -xAxis_.X * s + xAxis_.Z * c };
		yAxis_ = { yAxis_.X * c + yAxis_.Z * s, yAxis_.Y, -yAxis_.X * s + yAxis_.Z * c };

		return Normalize();
	}

	Rotation3D& Rotation3D::AddGlobalRotationZ(float degrees)// Treat like a 2D-rotation
	{
		const float rad = degrees * (float)RADIANS_PER_DEGREE;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		xAxis_ = { xAxis_.X * c - xAxis_.Y * s, xAxis_.X * s + xAxis_.Y * c, xAxis_.Z };
		yAxis_ = { yAxis_.X * c - yAxis_.Y * s, yAxis_.X * s + yAxis_.Y * c, yAxis_.Z };

		return Normalize();
	}	

	Rotation3D& Rotation3D::AddLocalRotationX(float degrees)// Treat localRotation as a point to rotate
	{
		const float rad = degrees * (float)RADIANS_PER_DEGREE;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		const Vec3f zAxis = yAxis_.GetCrossProduct(xAxis_);
		//yAxis_ = yAxis_ * s + z * (-c);  // localRotatX.Z = -c,  localRotX.Y = s
		yAxis_ = yAxis_ * c + zAxis * s;

		return Normalize();
	}

	Rotation3D& Rotation3D::AddLocalRotationY(float degrees) // Treat localRotation as a point to rotate
	{
		const float rad = degrees * (float)RADIANS_PER_DEGREE;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		const Vec3f zAxis = yAxis_.GetCrossProduct(xAxis_);
		xAxis_ = xAxis_ * c + zAxis * (-s); // localRotX.X = c, localRotX.Z = -s

		return Normalize();
	}

	Rotation3D& Rotation3D::AddLocalRotationZ(float degrees)// Treat localRotation as two points to rotate
	{
		const float rad = degrees * (float)RADIANS_PER_DEGREE;

		const float s = std::sinf(rad);
		const float c = std::cosf(rad);

		const Vec3f tempX = xAxis_;
		xAxis_ = xAxis_ * c + yAxis_ * s; // localRotX.X = c, localRotX.Y = s						   
		yAxis_ = tempX * (-s) + yAxis_ * c; // localRotY.X = -s, localRotY.Y = c;

		return Normalize();
	}

	void Rotation3D::RotatePoint(Vec3f& point) const 
	{
		point = (xAxis_ * point.X) + (yAxis_ * point.Y) + (GetZAxis() * point.Z);
	}

	void Rotation3D::RotatePoint(Vec3& point) const 
	{
		// Convert to Vector3d
		Vec3 xAxis(xAxis_);
		Vec3 yAxis(yAxis_);
		Vec3 axisZ(GetZAxis());

		point = (xAxis * point.X) + (yAxis * point.Y) + (axisZ * point.Z);
	}

	Rotation3D& Rotation3D::Normalize() 
	{
		xAxis_.Normalize();
		const float overlapOnX = yAxis_.GetDotProduct(xAxis_);
		yAxis_ -= (xAxis_ * overlapOnX);
		yAxis_.Normalize();

		return *this;
	}






	Transform3D::Transform3D(): 
		Position(0.0, 0.0, 0.0), 
		Scale(1.0, 1.0, 1.0), 
		Rotation() 
	{
	}

	Transform3D::Transform3D(const Vec3& position, Vec3f scale, const Rotation3D& rotation) :Position(position), Scale(scale), Rotation(rotation) {}

	void Transform3D::TransformPoints(Vec3* points, int count) const 
	{
		// Convert to Vec3 for repeated use:
		const Vec3 scale(Scale);
		const Vec3 xAxis(Rotation.GetXAxis());
		const Vec3 yAxis(Rotation.GetYAxis());
		const Vec3 zAxis(Rotation.GetZAxis());

		for (int i = 0; i < count; i++)
		{
			// Scale the mesh point
			points[i] = points[i] * scale;

			// Rotate the mesh point
			points[i] = xAxis * points[i].X + yAxis * points[i].Y + zAxis * points[i].Z;

			// Position the mesh point
			points[i] += Position;
		}
	}

	void Transform3D::TransformPoint(Vec3& point) const 
	{
		// Convert to Vector3d for use:
		const Vec3 scale(Scale);
		const Vec3 xAxis(Rotation.GetXAxis());
		const Vec3 yAxis(Rotation.GetYAxis());
		const Vec3 zAxis(Rotation.GetZAxis());

		// Scale the point:
		point *= scale;

		// Rotate the point:
		point = xAxis * point.X + yAxis * point.Y + zAxis * point.Z;

		// Position the point:
		point += Position;
	}

	void Transform3D::InverseTransformPoints(Vec3* points, int count) const
	{
		// Precompute inverse scale. DivideSafe() keeps it finite.
		Vec3 invScale(GetSafeDivision(1.0, (double)Scale.X), GetSafeDivision(1.0, (double)Scale.Y), GetSafeDivision(1.0, (double)Scale.Z));
		Rotation3D inverseRotation = Rotation.GetInverse();

		for (int i = 0; i < count; i++)
		{
			// Reverse translation
			points[i] -= Position;

			// Reverse rotation
			inverseRotation.RotatePoint(points[i]);

			// Reverse scale
			points[i] *= invScale;
		}
	}

	void Transform3D::InverseTransformPoint(Vec3& point) const
	{
		// Reverse translation
		point -= Position;

		// Reverse rotation
		Rotation.GetInverse().RotatePoint(point);

		// Reverse scale
		point.X = GetSafeDivision(point.X, (double)Scale.X);
		point.Y = GetSafeDivision(point.Y, (double)Scale.Y);
		point.Z = GetSafeDivision(point.Z, (double)Scale.Z);
	}
}