#pragma once

//#include <SDL_stdinc.h>

//#include <SDL.h>

//#include <vector>
#include <limits>
#include <cmath>


namespace pix
{

	//################################################################################ CONSTANTS #############################################################

	constexpr double PI = 3.14159265358979323846;  // More than enough precision

	constexpr double RADIANS_PER_DEGREE = PI/180.0;
	
	constexpr double DEGREES_PER_RADIAN = 180.0/PI;

	//################################################################################ GENERAL FUNCTIONS #############################################################

	
	// Returns max/lowest for nonzero numerator and zero denominator; returns 0 for 0/0 and other undefined results(NaN).
	// Note: 0/0 = 0 is sound in practice for the primary purpose of games (e.g. the speed of a static object is still zero, regardless of time delta).
	inline float GetSafeDivision(float numerator, float denominator)
	{
		if (denominator == 0.0f)
		{	
			if (numerator > 0.0f) return std::numeric_limits<float>::max();		
			if (numerator < 0.0f) return std::numeric_limits<float>::lowest();

			return 0.0f; // 0/0 = 0
		}

		float result = numerator / denominator;

		if (!std::isfinite(result))
		{
			if (result > 0.0f) return std::numeric_limits<float>::max();
			if (result < 0.0f) return std::numeric_limits<float>::lowest();

			return 0.0f;
		}

		return result;
	}
	
	// Returns max/lowest for nonzero numerator and zero denominator; returns 0 for 0/0 and other undefined results(NaN).
	// Note: 0/0 = 0 is sound in practice for the primary purpose of games (e.g. the speed of a static object is still zero, regardless of time delta).
	inline double GetSafeDivision(double numerator, double denominator)
	{
		if (denominator == 0.0)
		{
			if (numerator > 0.0) return std::numeric_limits<double>::max();
			if (numerator < 0.0) return std::numeric_limits<double>::lowest();

			return 0.0; // 0/0 = 0
		}

		double result = numerator / denominator;

		if (!std::isfinite(result))
		{
			if (result > 0.0) return std::numeric_limits<double>::max();
			if (result < 0.0) return std::numeric_limits<double>::lowest();

			return 0.0;
		}

		return result;
	}

	// Returns a clamped value between min and max: min must be <= max
	template<typename T> T GetClampedValue(T value, T min, T max)
	{
		if (value < min) return min;

		if (value > max) return max;

		return value;
	}

	
	//################################################################################ 2D TYPES #############################################################

	// Generic 2D vector
	template<typename T> struct Vector2
	{
		T X;
		T Y;

		// Any value can be valid
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

		// 1. Has to be a non-member function to enable type conversion on all arguments (we want to allow conversions only for built-in types (factor), for consistency) 
		// 2. The function has to be implemented inside the class so that it is already defined when called. Only then implicit type deduction applies, not for templates. 
		// 3. Only friend non-member functions can be declared inside a class. 
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

		T GetSquareLength() const 
		{
			return X * X + Y * Y;
		}

		// Tries to scale the vector to the unit length. The zero-vector is assigned if length is zero.
		Vector2& Normalize() 
		{
			const T length = GetLength();

			if (length > (T)0)
			{
				X /= length;
				Y /= length;
			}
			else
			{
				X = (T)0;
				Y = (T)0;
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
			return Vector2<T2>( (T2)X, (T2)Y);
		}

	};
	using Vec2 = Vector2<double>;
	using Vec2f = Vector2<float>;
	using Vec2i = Vector2<int>;


	// Rotation2D represents a 2D-rotation by a 2D unit vector to enable efficient rotation operations.
	// Rotation2D ensures the rotation is always valid (the unity vector is maintained). 
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

		

		friend Rotation2D GetInterpolated(Rotation2D prevRotation, Rotation2D rotation, float interpolation);

	private:

		// Used for efficient implementation of GetInverse() and Interpolate()
		Rotation2D(float x, float y); 

		Vec2f xAxis_;
	};

	// Transform2D represents a 2D affine transform (scale, rotation, translation) in local or world space.
    // Transform order: scale -> rotate -> translate.
	struct Transform2D
	{
		Transform2D();

		Transform2D(Vec2 position, Vec2f scale = { 1.0f, 1.0f }, Rotation2D rotation = Rotation2D());

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



	//################################################################################### 3D TYPES ##################################################################

	// Generic 3D vector
	template<typename T> struct Vector3
	{
		T X;
		T Y;
		T Z;

		Vector3() = default;

		Vector3(T x, T y, T z): X(x), Y(y), Z(z) {}

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

		// 1. Has to be a non-member function to enable type conversion on all arguments (we want to allow conversions only for built-in types (factor), for consistency) 
		// 2. The function has to be implemented inside the class so that it is already defined when called. Only then implicit type deduction applies, not for templates. 
		// 3. Only friend non-member functions can be declared inside a class. 
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

		T GetDotProduct(const Vector3& v) const 
		{
			return X * v.X + Y * v.Y + Z * v.Z;
		}

		// To use in a left-handed coordinate system, just use v as is and negate the result (or flip operands: a x b = -(b x a)).
		// v is applied on the right side. 
		Vector3 GetCrossProduct(const Vector3& v) const
		{
			const T x = Y * v.Z - Z * v.Y;
			const T y = Z * v.X - X * v.Z;
			const T z = X * v.Y - Y * v.X;

			return Vector3(x, y, z);
		}

		T GetLength() const 
		{
			return std::sqrt(X * X + Y * Y + Z * Z);
		}

		T GetSquareLength() const
		{
			return X * X + Y * Y + Z * Z;
		}

		// Tries to scale the vector to the unit length. The zero-vector is assigned if length is zero.
		Vector3& Normalize() 
		{
			const T length = GetLength();

			if (length > 0.0f)
			{
				X /= length;
				Y /= length;
				Z /= length;
			}
			else
			{
				X = 0.0f;
				Y = 0.0f;
				Z = 0.0f;
			}

			return *this;
		}

		// For explicit type conversion
		template<typename T2> explicit operator Vector3<T2>() const 
		{
			return Vector3<T2>( (T2)X, (T2)Y, (T2)Z);
		}

	};
	using Vec3 = Vector3<double>;
	using Vec3f = Vector3<float>;


	// Rotation3D represents a left-handed 3D-rotation by two perpendicular 3D unit vectors to enable efficient rotation operations.
	// Rotation3D ensures the rotation is always valid (maintains perpendicular unit vectors). 
	// Positive angles are added in mathematical (counterclockwise) direction).
	class Rotation3D
	{
	public:

		Rotation3D();

		void SetToIdentity();


		// The projection of the global coordinate system on this rotation
		Rotation3D& Inverse();

		// In effect globalRotation.RotatePoint() applied to the xAxis and yAxis 
		Rotation3D& AddGlobalRotation(const Rotation3D& globalRotation); 

		// In effect RotatePoint() applied to the localRotation axes
		Rotation3D& AddLocalRotation(const Rotation3D& localRotation);// Treat localRotation as two rotated points

		Rotation3D& AddGlobalRotation(Vec3f rotAxis, float degrees);

		Rotation3D& AddGlobalRotationX(float degrees); 

		Rotation3D& AddGlobalRotationY(float degrees);

		Rotation3D& AddGlobalRotationZ(float degrees);

		Rotation3D& AddLocalRotationX(float degrees);

		Rotation3D& AddLocalRotationY(float degrees); 

		Rotation3D& AddLocalRotationZ(float degrees);



		void RotatePoint(Vec3& point) const;

		void RotatePoint(Vec3f& point) const;

		void RotatePoints(Vec3* points, int count); // TODO

		void RotatePoints(Vec3f* points, int count); // TODO


		void InverseRotatePoint(Vec3& point) const;

		void InverseRotatePoint(Vec3f& point) const;

		void InverseRotatePoints(Vec3* points, int count); // TODO

		void InverseRotatePoints(Vec3f* points, int count); // TODO


		// The projection of globalRotation on this rotation
		Rotation3D GetLocalRotation(const Rotation3D& globalRotation) const;

		Rotation3D GetInverse() const; 

		Vec3f GetXAxis() const;

		Vec3f GetYAxis() const;

		Vec3f GetZAxis() const;
		


		friend Rotation3D GetInterpolated(const Rotation3D& prevRotation, const Rotation3D& rotation, float interpolation);

	private:

		Rotation3D(const Vec3f& rotationX, const Vec3f& rotationY);

		Rotation3D& Normalize();

		Vec3f xAxis_;
		Vec3f yAxis_;
	};

	// Transform3D represents a 3D affine transform (scale, rotation, translation) in local or world space.
	// Transform order: scale -> rotate -> translate.
	class Transform3D
	{
	public:

		Transform3D();

		Transform3D(const Vec3& position, Vec3f scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());


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


	//################################################################ UTILS FOR 2D TYPES ###################################################


	
	template<typename T> inline Vector2<T> GetSafeDivision(const Vector2<T>& numeratorVector, const Vector2<T>& denominatorVector) 
	{
		return Vector2<T>(GetSafeDivision(numeratorVector.X, denominatorVector.X), GetSafeDivision(numeratorVector.Y, denominatorVector.Y));
	}
	
	template<typename T> inline Vector2<T> GetSafeDivision(const Vector2<T>& numeratorVector, T denominator)
	{
		return Vector2<T>(GetSafeDivision(numeratorVector.X, denominator), GetSafeDivision(numeratorVector.Y, denominator));
	}

	template<typename T> inline Vector2<T> GetInterpolatedUnchecked(const Vector2<T>& startVector, const Vector2<T>& endVector, T alpha) 
	{
		return startVector + (endVector - startVector) * alpha;
	}

	inline Rotation2D GetInterpolated(Rotation2D startRotation, Rotation2D endRotation, float alpha) 
	{
		const Vec2f xAxis = endRotation.GetXAxis();
		const Vec2f previousXAxis = startRotation.GetXAxis();

		alpha = GetClampedValue(alpha, 0.0f, 1.0f);

		// If rotation is more than 90° apart, return the target rotation directly
		if (xAxis.GetDotProduct(previousXAxis) < 0.0f)
			return Rotation2D(endRotation);

		Vec2f interpolatedRotX = GetInterpolatedUnchecked(previousXAxis, xAxis, alpha);
		interpolatedRotX.Normalize();

		return Rotation2D(interpolatedRotX.X, interpolatedRotX.Y);
	}

	
	template<typename T> inline void RotatePointUnchecked(const Vector2<T>& xAxis, Vector2<T>& point) 
	{
		point = { xAxis.X * point.X - xAxis.Y * point.Y,
				  xAxis.Y * point.X + xAxis.X * point.Y };
	}
	

	//################################################################ UTILS FOR 3D TYPES ###################################################


	template<typename T> inline Vector3<T> GetSafeDivision(const Vector3<T>& numeratorVector, const Vector3<T>& denominatorVector)
	{
		return Vector3<T>(GetSafeDivision(numeratorVector.X, denominatorVector.X), GetSafeDivision(numeratorVector.Y, denominatorVector.Y), GetSafeDivision(numeratorVector.Z, denominatorVector.Z));
	}

	template<typename T> inline Vector3<T> GetSafeDivision(const Vector3<T>& numeratorVector, T denominator)
	{
		return Vector3<T>(GetSafeDivision(numeratorVector.X, denominator), GetSafeDivision(numeratorVector.Y, denominator), GetSafeDivision(numeratorVector.Z, denominator));
	}

	template<typename T> inline Vector3<T> GetInterpolatedUnchecked(const Vector3<T>& startVector, const Vector3<T>& endVector, T alpha)
	{
		return startVector + (endVector - startVector) * alpha;
	}

	inline Rotation3D GetInterpolated(const Rotation3D& prevRotation, const Rotation3D& rotation, float interpolationAlpha) 
	{
		const Vec3f& xAxis = rotation.GetXAxis();
		const Vec3f& yAxis = rotation.GetYAxis();
		const Vec3f& previousXAxis = prevRotation.GetXAxis();
		const Vec3f& previousYAxis = prevRotation.GetYAxis();

		interpolationAlpha = GetClampedValue(interpolationAlpha, 0.0f, 1.0f);

		// 90+ degrees is more than enough for interpolation to not make much sense
		if ((xAxis.GetDotProduct(previousXAxis) < 0.0f) || (yAxis.GetDotProduct(previousYAxis) < 0.0f))
			return Rotation3D(rotation);

		const Vec3f interpolatedRotX = GetInterpolatedUnchecked(previousXAxis, xAxis, interpolationAlpha);
		const Vec3f interpolatedRotY = GetInterpolatedUnchecked(previousYAxis, yAxis, interpolationAlpha);

		return Rotation3D(interpolatedRotX, interpolatedRotY);
	}




}
