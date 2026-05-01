#pragma once

#include <cmath>

namespace pix
{

	// ################################################################################ CONSTANTS #############################################################


	constexpr double PI = 3.14159265358979323846;  // More than enough precision

	constexpr double RADIANS_PER_DEGREE = PI / 180.0;
	
	constexpr double DEGREES_PER_RADIAN = 180.0 / PI;
	

	// ################################################################################ GENERAL FUNCTIONS #############################################################


	// Returns the linear interpolation between startValue and endValue using unclamped interpolationAlpha (expected range [0, 1]).
	template<typename T> inline T GetInterpolatedUnchecked(T startValue, T endValue, T interpolationAlpha)
	{
		return startValue + (endValue - startValue) * interpolationAlpha;
	}

	// Returns a clamped value between min and max: min <= max must hold
	template<typename T> T GetClamped(T value, T min, T max)
	{
		if (value < min) return min;

		if (value > max) return max;

		return value;
	}

	// Returns max/lowest for nonzero numerator and zero denominator; returns 0 for 0/0 and other undefined results(NaN).
	// Note: 0/0 = 0 is sound in practice for the primary purpose of games (e.g. the speed of a static object is still zero, regardless of time delta).
	float GetSafeDivision(float numerator, float denominator);

	// Returns max/lowest for nonzero numerator and zero denominator; returns 0 for 0/0 and other undefined results(NaN).
	// Note: 0/0 = 0 is sound in practice for the primary purpose of games (e.g. the speed of a static object is still zero, regardless of time delta).
	double GetSafeDivision(double numerator, double denominator);


	// ################################################################################ VECTORS #############################################################


	// Generic 2D vector
	template<typename T> struct Vector2
	{
		T X;
		T Y;

		// Intentionally left uninitialized: any value can be valid
		Vector2() = default; 

		Vector2(T x, T y): X(x), Y(y) {}

		Vector2 operator+ (const Vector2& other) const 
		{
			return Vector2(X + other.X, Y + other.Y);
		}

		Vector2& operator+= (const Vector2& other)
		{
			X += other.X;
			Y += other.Y;

			return *this;
		}

		Vector2 operator-() const 
		{
			return Vector2(-X, -Y);
		}

		Vector2 operator- (const Vector2& other) const 
		{
			return Vector2(X - other.X, Y - other.Y);
		}

		Vector2& operator-= (const Vector2& other) 
		{
			X -= other.X;
			Y -= other.Y;

			return *this;
		}

		Vector2 operator* (const Vector2& other) const 
		{
			return Vector2(X * other.X, Y * other.Y);
		}

		Vector2& operator*= (const Vector2& other) 
		{
			X *= other.X;
			Y *= other.Y;

			return *this;
		}

		Vector2 operator* (T factor) const
		{
			return Vector2(X * factor, Y * factor);
		}

		// Non-member to allow scalar * vector syntax.
		// Defined here so it is available during template instantiation.
		friend Vector2 operator* (T factor, const Vector2& vector) 
		{
			return vector * factor;
		}

		Vector2& operator*= (T factor) 
		{
			X *= factor;
			Y *= factor;

			return *this;
		}

		// Does not guard against divide-by-zero
		Vector2 operator/ (const Vector2& other) const 
		{
			return Vector2(X / other.X, Y / other.Y);
		}

		// Does not guard against divide-by-zero
		Vector2& operator/= (const Vector2& other) 
		{
			X /= other.X;
			Y /= other.Y;

			return *this;
		}

		// Does not guard against divide-by-zero
		Vector2 operator/ (T divisor) const 
		{
			return Vector2(X / divisor, Y / divisor);
		}

		// Does not guard against divide-by-zero
		Vector2& operator/= (T divisor) 
		{
			X /= divisor;
			Y /= divisor;

			return *this;
		}

		bool operator==(const Vector2& other) const 
		{
			return X == other.X && Y == other.Y;
		}

		bool operator!= (const Vector2& other) const 
		{
			return X != other.X || Y != other.Y;
		}

		// Returns the normal in mathematical (counter-clockwise) direction with the same length.
		Vector2 GetNormal() const 
		{
			return Vector2(-Y, X);
		}

		T GetLength() const 
		{
			return std::sqrt(X * X + Y * Y);
		}

		T GetSquaredLength() const 
		{
			return X * X + Y * Y;
		}

		// Tries to scale the vector to the unit length. The zero-vector is assigned if length is zero.
		Vector2& Normalize() 
		{
			const T length = GetLength();

			if (length > T(0))
			{
				X /= length;
				Y /= length;
			}
			else
			{
				X = T(0);
				Y = T(0);
			}

			return *this;
		}

		T GetDotProduct(const Vector2& other) const
		{
			return X * other.X + Y * other.Y;
		}

		// For explicit type conversion
		template<typename T2> explicit operator Vector2<T2>() const // Explicit: no mixed-type arithmetic to prevent mistakes 
		{
			return Vector2<T2>(T2(X), T2(Y));
		}

	};

	using Vec2 = Vector2<double>;
	using Vec2f = Vector2<float>;
	using Vec2i = Vector2<int>; // Intended for indexing and storage use, not well suited for operations that involve scaling

	// Generic 3D vector
	template<typename T> struct Vector3
	{
		T X;
		T Y;
		T Z;

		// Intentionally left uninitialized: any value can be valid
		Vector3() = default;

		Vector3(T x, T y, T z) : X(x), Y(y), Z(z) {}

		Vector3 operator+ (const Vector3& other) const
		{
			return Vector3(X + other.X, Y + other.Y, Z + other.Z);
		}

		Vector3& operator+= (const Vector3& other)
		{
			X += other.X;
			Y += other.Y;
			Z += other.Z;

			return *this;
		}

		Vector3 operator-() const
		{
			return Vector3(-X, -Y, -Z);
		}

		Vector3 operator- (const Vector3& other) const
		{
			return Vector3(X - other.X, Y - other.Y, Z - other.Z);
		}

		Vector3& operator-= (const Vector3& other)
		{
			X -= other.X;
			Y -= other.Y;
			Z -= other.Z;

			return *this;
		}

		Vector3 operator* (const Vector3& other) const
		{
			return Vector3(X * other.X, Y * other.Y, Z * other.Z);
		}

		Vector3& operator*= (const Vector3& other)
		{
			X *= other.X;
			Y *= other.Y;
			Z *= other.Z;

			return *this;
		}

		Vector3 operator* (T factor) const
		{
			return Vector3(X * factor, Y * factor, Z * factor);
		}

		// Non-member to allow scalar * vector syntax.
		// Defined here so it is available during template instantiation.
		friend Vector3 operator* (T factor, const Vector3& vector)
		{
			return vector * factor;
		}

		Vector3& operator*= (T factor)
		{
			X *= factor;
			Y *= factor;
			Z *= factor;

			return *this;
		}

		// Does not guard against divide-by-zero
		Vector3 operator/ (const Vector3& other) const
		{
			return Vector3(X / other.X, Y / other.Y, Z / other.Z);
		}

		// Does not guard against divide-by-zero
		Vector3& operator/= (const Vector3& other)
		{
			X /= other.X;
			Y /= other.Y;
			Z /= other.Z;

			return *this;
		}

		// Does not guard against divide-by-zero
		Vector3 operator/ (T divisor) const
		{
			return Vector3(X / divisor, Y / divisor, Z / divisor);
		}

		// Does not guard against divide-by-zero
		Vector3& operator/= (T divisor)
		{
			X /= divisor;
			Y /= divisor;
			Z /= divisor;

			return *this;
		}

		bool operator==(const Vector3& other) const
		{
			return X == other.X && Y == other.Y && Z == other.Z;
		}

		bool operator!= (const Vector3& other) const
		{
			return X != other.X || Y != other.Y || Z != other.Z;
		}

		T GetDotProduct(const Vector3& other) const
		{
			return X * other.X + Y * other.Y + Z * other.Z;
		}

		// Standard cross product
		Vector3 GetCrossProduct(const Vector3& other) const
		{
			return Vector3(Y * other.Z - Z * other.Y, Z * other.X - X * other.Z, X * other.Y - Y * other.X);
		}

		T GetLength() const
		{
			return std::sqrt(X * X + Y * Y + Z * Z);
		}

		T GetSquaredLength() const
		{
			return X * X + Y * Y + Z * Z;
		}

		// Tries to scale the vector to the unit length. The zero-vector is assigned if length is zero.
		Vector3& Normalize()
		{
			const T length = GetLength();

			if (length > T(0))
			{
				X /= length;
				Y /= length;
				Z /= length;
			}
			else
			{
				X = T(0);
				Y = T(0);
				Z = T(0);
			}

			return *this;
		}

		// For explicit type conversion
		template<typename T2> explicit operator Vector3<T2>() const
		{
			return Vector3<T2>(T2(X), T2(Y), T2(Z));
		}

	};

	using Vec3 = Vector3<double>;
	using Vec3f = Vector3<float>;
	using Vec3i = Vector3<int>; // Intended for indexing and storage use, not well suited for operations that involve scaling


	// ############################################################### VECTOR OPERATIONS ###################################################


	// Returns a linearly interpolated Vector2 between the startVector and the endVector using unclamped interpolationAlpha (expected range [0, 1]).
	template<typename T> inline Vector2<T> GetInterpolatedUnchecked(const Vector2<T>& startVector, const Vector2<T>& endVector, T interpolationAlpha)
	{
		return startVector + (endVector - startVector) * interpolationAlpha;
	}

	// Componentwise GetSafeDivision(float) or GetSafeDivision(double)
	template<typename T> inline Vector2<T> GetSafeDivision(const Vector2<T>& numeratorVector, const Vector2<T>& denominatorVector)
	{
		return Vector2<T>(GetSafeDivision(numeratorVector.X, denominatorVector.X), GetSafeDivision(numeratorVector.Y, denominatorVector.Y));
	}

	// Componentwise GetSafeDivision(float) or GetSafeDivision(double)
	template<typename T> inline Vector2<T> GetSafeDivision(const Vector2<T>& numeratorVector, T denominator)
	{
		return Vector2<T>(GetSafeDivision(numeratorVector.X, denominator), GetSafeDivision(numeratorVector.Y, denominator));
	}

	// Returns a linearly interpolated Vector3 between the startVector and the endVector using unclamped interpolationAlpha (expected range [0, 1]).
	template<typename T> inline Vector3<T> GetInterpolatedUnchecked(const Vector3<T>& startVector, const Vector3<T>& endVector, T interpolationAlpha)
	{
		return startVector + (endVector - startVector) * interpolationAlpha;
	}

	// Componentwise GetSafeDivision(float) or GetSafeDivision(double)
	template<typename T> inline Vector3<T> GetSafeDivision(const Vector3<T>& numeratorVector, const Vector3<T>& denominatorVector)
	{
		return Vector3<T>(GetSafeDivision(numeratorVector.X, denominatorVector.X), GetSafeDivision(numeratorVector.Y, denominatorVector.Y), GetSafeDivision(numeratorVector.Z, denominatorVector.Z));
	}

	// Componentwise GetSafeDivision(float) or GetSafeDivision(double)
	template<typename T> inline Vector3<T> GetSafeDivision(const Vector3<T>& numeratorVector, T denominator)
	{
		return Vector3<T>(GetSafeDivision(numeratorVector.X, denominator), GetSafeDivision(numeratorVector.Y, denominator), GetSafeDivision(numeratorVector.Z, denominator));
	}


	// ################################################################################### ROTATIONS ##################################################################


	// Rotation2D represents a 2D rotation using a single unit vector (its local X axis). 
	// The local Y axis is derived automatically as the perpendicular (normal) to X axis, so the two axes always form a perpendicular coordinate frame.
	//
	// Positive rotation angles are counterclockwise.
	//
	// The rotation is always kept valid and numerically stable:
	// after every mutating operation (not needed for Inverse()) the X axis is re-normalized to prevent floating-point drift from accumulating over time.
	//
	// Philosophy:
	// The rotation is just the direction of the local X axis. 
	// Rotating a point means expressing it in this rotated coordinate frame so the rotation operations become performant.
	class Rotation2D
	{
	public:

		Rotation2D();

		Rotation2D(float degrees);



		void Set(float degrees);

		Rotation2D& AddRotation(float deltaDegrees);

		Rotation2D& AddRotation(Rotation2D deltaRotation);

		void SetToIdentity();

		Rotation2D& Inverse();
		


		void RotatePoint(Vec2f& point) const;

		void InverseRotatePoint(Vec2f& point) const;

		void RotatePoint(Vec2& point) const;

		void InverseRotatePoint(Vec2& point) const;

		Vec2f GetXAxis() const;

		// Returns the normal vector to xAxis in mathematical direction
		Vec2f GetYAxis() const;

		// Returns the angle in degrees
		float GetAngle() const;

		Rotation2D GetInverse() const;

		

		friend Rotation2D GetInterpolated(Rotation2D startRotation, Rotation2D endRotation, float interpolationAlpha);

	private:

		// Used for efficient implementation of GetInverse() and Interpolate()
		Rotation2D(float x, float y); 

		Vec2f xAxis_;
	};

	// Rotation3D represents a 3D rotation using two perpendicular unit vectors (its local X and Y axes).
    // The third axis (Z) is derived automatically via the cross product, so the three axes always form
    // a right-handed perpendicular coordinate frame (in linear algebra: an orthonormal basis).
	// 
	// Positive rotation angles follow the mathematical rule:
	// counterclockwise when looking along the positive direction of the rotation axis.
	// 
	// The rotation is always kept valid and numerically stable:
	// after every mutating operation the axes are re-normalized to prevent floating-point drift from accumulating over time.
	//
	// Philosophy:
	// The rotation is represented by the direction of the local X and Y axes.
	// Rotating a point means expressing that point in this rotated coordinate frame so the rotation operations become performant.
	class Rotation3D
	{
	public:

		Rotation3D();

		void SetToIdentity();


		// Achieved by projecting the global coordinate system onto this rotation's axes (in linear algebra: equivalent to transposing the orthonormal basis)
		Rotation3D& Inverse();

		// Equivalent to globalRotation.RotatePoint() applied to the xAxis and yAxis
		Rotation3D& AddGlobalRotation(const Rotation3D& globalRotation);

		Rotation3D& AddGlobalRotation(Vec3f rotAxis, float degrees);

		Rotation3D& AddGlobalRotationX(float degrees);

		Rotation3D& AddGlobalRotationY(float degrees);

		Rotation3D& AddGlobalRotationZ(float degrees);

		// Equivalent to RotatePoint() applied to the localRotation axes
		Rotation3D& AddLocalRotation(const Rotation3D& localRotation);

		Rotation3D& AddLocalRotationX(float degrees);

		Rotation3D& AddLocalRotationY(float degrees);

		Rotation3D& AddLocalRotationZ(float degrees);



		void RotatePoint(Vec3& point) const;

		void RotatePoints(Vec3* points, int count) const;

		void InverseRotatePoint(Vec3& point) const;

		void InverseRotatePoints(Vec3* points, int count) const;

		void RotatePoint(Vec3f& point) const;

		void RotatePoints(Vec3f* points, int count) const;

		void InverseRotatePoint(Vec3f& point) const;

		void InverseRotatePoints(Vec3f* points, int count) const;



		// Returns globalRotation expressed in this rotation's local coordinate space (equivalent to adding the inverse to globalRotation).
		Rotation3D GetLocalRotationOf(const Rotation3D& globalRotation) const;

		Rotation3D GetInverse() const;

		Vec3f GetXAxis() const;

		Vec3f GetYAxis() const;

		Vec3f GetZAxis() const;



		friend Rotation3D GetInterpolated(const Rotation3D& startRotation, const Rotation3D& endRotation, float interpolationAlpha);

	private:

		Rotation3D(Vec3f xAxis, Vec3f yAxis); // Used for AddGlobalRotation(Vec3f rotAxis, float degrees)

		Rotation3D& Normalize();

		Vec3f xAxis_;
		Vec3f yAxis_;
	};


	// ############################################################### ROTATION OPERATIONS ###################################################


	// Returns a linearly interpolated Rotation2D by interpolating between the rotation axes using interpolationAlpha (internally clamped to [0.0f, 1.0f]).
	// Works most accurately for small angle differences of a few degrees. If the local X axes differ by more than 60 degrees, endRotation is returned.
	Rotation2D GetInterpolated(Rotation2D startRotation, Rotation2D endRotation, float interpolationAlpha);

	// xAxis represents the raw (and eventually scaled) rotation vector by which the provided point is rotated.
	template<typename T> inline void RotatePointUnchecked(const Vector2<T>& xAxis, Vector2<T>& point)
	{
		point = Vector2<T>(xAxis.X * point.X - xAxis.Y * point.Y,
			               xAxis.Y * point.X + xAxis.X * point.Y);
	}

	// Returns a linearly interpolated Rotation3D by interpolating between the rotation axes using interpolationAlpha (internally clamped to [0.0f, 1.0f]).
	// Works most accurately for small angle differences of a few degrees. If either axis differs by more than 60 degrees, endRotation is returned.
	Rotation3D GetInterpolated(const Rotation3D& startRotation, const Rotation3D& endRotation, float interpolationAlpha);


	// ################################################################################### TRANSFORMS ##################################################################


	// Transform2D represents the position, rotation, and scale of a 2D object in either local or world space.
	// Transform order: scale -> rotate -> translate.
	//
	// Philosophy:
	// Transform2D contains only the minimal data required to describe the position, orientation, and scale of a 2D object in a given space.
	// 
	// It has no knowledge of hierarchy or parent-child relationships.
	// Hierarchical structure is defined externally (e.g., by sprite nodes),
	// keeping this type lightweight and reusable without additional overhead.
	struct Transform2D
	{
		Transform2D();

		Transform2D(Vec2 position, Vec2f scale = Vec2f(1.0f, 1.0f), Rotation2D rotation = Rotation2D());

		// Applies the transform to points.
		// points must be non-null when count > 0. Caller must ensure count does not exceed the array length.
		void TransformPoints(Vec2* points, int count) const;

		void TransformPoint(Vec2& point) const;

		// Applies the inverse transform to points.
		// points must be non-null when count > 0. Caller must ensure count does not exceed the array length.
		void InverseTransformPoints(Vec2* points, int count) const;

		void InverseTransformPoint(Vec2& point) const;

		

		Vec2 Position;
		Vec2f Scale;
		Rotation2D Rotation;
	};



	// Transform3D represents the position, rotation, and scale of a 3D object in either local or world space.
	// Transform order: scale -> rotate -> translate.
	//
	// Philosophy:
	// Transform3D contains only the minimal data required to describe the position, orientation, and scale of a 3D object in a given space.
	// 
	// It has no knowledge of hierarchy or parent-child relationships.
	// Hierarchical structure is defined externally (e.g., by sprite nodes),
	// keeping this type lightweight and reusable without additional overhead.
	struct Transform3D
	{
		Transform3D();

		Transform3D(const Vec3& position, Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f), const Rotation3D& rotation = Rotation3D());

		// Applies the transform to points.
		// points must be non-null when count > 0. Caller must ensure count does not exceed the array length.
		void TransformPoints(Vec3* points, int count) const;

		void TransformPoint(Vec3& point) const;

		// Applies the inverse transform to points.
		// points must be non-null when count > 0. Caller must ensure count does not exceed the array length.
		void InverseTransformPoints(Vec3* points, int count) const;

		void InverseTransformPoint(Vec3& point) const;

		Vec3 Position;
		Vec3f Scale;
		Rotation3D Rotation;
	};

	
	// ############################################################### TRANSFORM OPERATIONS ###################################################


	// Returns an interpolated Transform2D by interpolating the position, scale and rotation between startTransform and endTransform with interpolationAlpha (internally clamped to [0.0f, 1.0f]).
    // It is functionally equivalent to calling the linear interpolation functions manually on the transform members.
	Transform2D GetInterpolated(const Transform2D& startTransform, const Transform2D& endTransform, float interpolationAlpha);

	// Returns an interpolated Transform3D by interpolating the position, scale and rotation between startTransform and endTransform with interpolationAlpha (internally clamped to [0.0f, 1.0f]).
    // It is functionally equivalent to calling the linear interpolation functions manually on the transform members.
	Transform3D GetInterpolated(const Transform3D& startTransform, const Transform3D& endTransform, float interpolationAlpha);
}
