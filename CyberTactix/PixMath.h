#pragma once

//#include <SDL_stdinc.h>

//#include <SDL.h>

//#include <vector>
#include <limits>
#include <cmath>


namespace pix
{

	//################################################################################ CONSTANTS #############################################################


	constexpr double PI_D = 3.14159265358979323846;  // More than enough precision
	constexpr float  PI_F = 3.14159265358979323846f;

	constexpr double RADIANS_PER_DEGREE_D = PI_D / 180.0;
	constexpr float  RADIANS_PER_DEGREE_F = PI_F / 180.0f;

	constexpr double DEGREES_PER_RADIAN_D = 180.0 / PI_D;
	constexpr float  DEGREES_PER_RADIAN_F = 180.0f / PI_F;


	//################################################################################ GENERAL FUNCTIONS #############################################################

	
	 
	// A safe division, also by zero, is guaranteed, returning max/min numbers, or zero in any undefined case.
	// Note: 0/0 = 0 is sound in practice for the primary purpose of games (e.g. the speed of a static object is still zero, regardless of time delta).
	inline float DivideSafe(float numerator, float denominator)
	{
		if (denominator == 0.0f)
		{	
			if (numerator > 0.0f)
				return std::numeric_limits<float>::max();
				
			if (numerator < 0.0f)
			   return std::numeric_limits<float>::lowest();

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
	
	// A safe division, also by zero, is guaranteed, returning max/min numbers, or zero in any undefined case.
	// Note: 0/0 = 0 is sound in practice for the primary purpose of games (e.g. the speed of a static object is still zero, regardless of time delta).
	inline double DivideSafe(double numerator, double denominator)
	{
		if (denominator == 0.0)
		{
			if (numerator > 0.0)
				return std::numeric_limits<double>::max();

			if (numerator < 0.0)
				return std::numeric_limits<double>::lowest();

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


	template<typename T> struct Vector2
	{
		T X;
		T Y;

		Vector2() = default; // Is not initialized on purpose for potential performance gains and because any value can be valid

		Vector2(T x, T y): X(x), Y(y) {}

		Vector2 operator+ (const Vector2& other) const 
		{
			return Vector2(X + other.X, Y + other.Y);
		}

		Vector2& operator+= (const Vector2& other)
		{
			X += other.X;
			Y += other.Y;

			return (*this);
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

			return (*this);
		}

		Vector2 operator* (const Vector2& other) const 
		{
			return Vector2(X * other.X, Y * other.Y);
		}

		Vector2& operator*= (const Vector2& other) 
		{
			X *= other.X;
			Y *= other.Y;

			return (*this);
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

			return (*this);
		}

		Vector2 operator/ (const Vector2& other) const 
		{
			return Vector2(X / other.X, Y / other.Y);
		}

		Vector2& operator/= (const Vector2& other) 
		{
			X /= other.X;
			Y /= other.Y;

			return (*this);
		}

		Vector2 operator/ (T divisor) const 
		{
			return Vector2(X / divisor, Y / divisor);
		}

		Vector2& operator/= (T divisor) 
		{
			X /= divisor;
			Y /= divisor;

			return (*this);
		}

		bool operator==(const Vector2& other) const 
		{
			return X == other.X && Y == other.Y;
		}

		bool operator!= (const Vector2& other) const 
		{
			return X != other.X || Y != other.Y;
		}

		/// <summary>
		/// Returns the same-length normal in mathematical (counter-clockwise) direction.
		/// </summary>
		/// <returns></returns>
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

		/// <summary>
		/// Tries to scale the vector to the unit length. The zero-vector will be assigned if length is zero.
		/// </summary>
		/// <returns></returns>
		Vector2& Normalize() 
		{
			const T length = GetLength();

			if (length > static_cast<T>(0))
			{
				X /= length;
				Y /= length;
			}
			else
			{
				X = static_cast<T>(0);
				Y = static_cast<T>(0);
			}

			return *this;
		}

		T DotProduct(const Vector2 other) const
		{
			return X * other.X + Y * other.Y;
		}

		// For explicit type conversion
		template<typename T2> explicit operator Vector2<T2>() const // Explicit: no mixed-type arithmetic to prevent mistakes 
		{
			return Vector2<T2>(X, Y);
		}

	};

	typedef Vector2<double> Vector2d;
	typedef Vector2<float>  Vector2f;
	typedef Vector2<int>    Vector2i;


	/// <summary>
	/// Represents a 2D-rotation by a 2D unit vector. Ensures the rotation is always valid. 
	/// </summary>
	class Rotation2D
	{
	public:

		Rotation2D();

		Rotation2D(float degrees);

		void SetToIdentity();

		void Set(float degrees);

		const Vector2f& GetXAxis() const;

		Vector2f GetYAxis() const;

		float GetAngle() const;

		Rotation2D& Inverse();

		Rotation2D GetInverse() const;

		Rotation2D& AddRotation(float deltaDegrees);

		Rotation2D& AddRotation(const Rotation2D& deltaRotation);


		Vector2f RotatePoint(const Vector2f& point) const
		{
			return { xAxis_.X * point.X - xAxis_.Y * point.Y,
					 xAxis_.Y * point.X + xAxis_.X * point.Y };
		}

		Vector2d RotatePoint(const Vector2d& point) const
		{
			Vector2d xAxis(xAxis_); // Convert to Vector2d for later use

			return { xAxis.X * point.X - xAxis.Y * point.Y,
					 xAxis.Y * point.X + xAxis.X * point.Y };
		}


		friend Rotation2D Interpolate(const Rotation2D& prevRotation, const Rotation2D& rotation, float interpolation);

	private:

		Rotation2D(float x, float y); // Used for efficient implementation of GetInverse() and Interpolate()

		Vector2f xAxis_;
	};


	class Transform2D
	{
	public:

		Vector2d   Position;
		Vector2f   Scale;
		Rotation2D Rotation;

		Transform2D();

		Transform2D(const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D());

		void ApplyToPoints(Vector2d* points, int count) const;

		void ApplyToPoint(Vector2d& point) const;

	};



	//################################################################################### 3D TYPES ##################################################################


	template<typename T> struct Vector3
	{
		T X;
		T Y;
		T Z;

		Vector3() {} 

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

			return (*this);
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

			return (*this);
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

			return (*this);
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

			return (*this);
		}

		Vector3 operator/ (const Vector3& other) const
		{
			return Vector3(X / other.X, Y / other.Y, Z / other.Z);
		}

		Vector3& operator/= (const Vector3& other) 
		{
			X /= other.X;
			Y /= other.Y;
			Z /= other.Z;

			return (*this);
		}

		Vector3 operator/ (T divisor) const
		{
			return Vector3(X / divisor, Y / divisor, Z / divisor);
		}

		Vector3& operator/= (T divisor)
		{
			X /= divisor;
			Y /= divisor;
			Z /= divisor;

			return (*this);
		}

		bool operator==(const Vector3& other) const 
		{
			return X == other.X && Y == other.Y && Z == other.Z;
		}

		bool operator!= (const Vector3& other) const 
		{
			return X != other.X || Y != other.Y || Z != other.Z;
		}

		T DotProduct(const Vector3& v) const 
		{
			return X * v.X + Y * v.Y + Z * v.Z;
		}

		Vector3 CrossProduct(const Vector3& v) const
		{
			const T x = Z * v.Y - Y * v.Z;
			const T y = X * v.Z - Z * v.X;
			const T z = Y * v.X - X * v.Y;

			// Cross-product assumes righ-handed coordinate system, we have a left-handed one. Flip result to account for that.
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

		//for explicit type conversion
		template<typename T2> explicit operator Vector3<T2>() const 
		{
			return Vector3<T2>(X, Y, Z);
		}

	};
	typedef Vector3<double> Vector3d;
	typedef Vector3<float>  Vector3f;



	class Rotation3D
	{
	public:

		Rotation3D();

		void SetToIdentity();

		const Vector3f& GetXAxis() const;

		const Vector3f& GetYAxis() const;

		Vector3f GetZAxis() const;

		Rotation3D& Inverse();// The projection of the global coordinate system on the member rotation

		Rotation3D GetInverse() const; // The projection of the global coordinate system on the member rotation

		Rotation3D ToLocalRotation(const Rotation3D& globalRotation) const;

		Rotation3D& AddGlobalRotation(const Rotation3D& globalRotation); // Treat member rotation as two points to rotate

		Rotation3D& AddLocalRotation(const Rotation3D& localRotation);// Treat localRotation as two rotated points

		Rotation3D& AddGlobalRotation(Vector3f rotAxis, float degrees);

		Rotation3D& AddGlobalRotationX(float degrees); // Treat like a 2D-rotation

		Rotation3D& AddGlobalRotationY(float degrees);// Treat like a 2D-rotation

		Rotation3D& AddGlobalRotationZ(float degrees);// Treat like a 2D-rotation

		Rotation3D& AddLocalRotationX(float degrees);// Treat localRotation as a point to rotate

		Rotation3D& AddLocalRotationY(float degrees); // Treat localRotation as a point to rotate

		Rotation3D& AddLocalRotationZ(float degrees);// Treat localRotation as two points to rotate

		Vector3f RotatePoint(const Vector3f& point) const;

		Vector3d RotatePoint(const Vector3d& point) const ;


		friend Rotation3D Interpolate(const Rotation3D& prevRotation, const Rotation3D& rotation, float interpolation);

	private:

		

		Rotation3D(const Vector3f& rotationX, const Vector3f& rotationY);

		Rotation3D& Normalize();

		Vector3f xAxis_;
		Vector3f yAxis_;

	};


	class Transform3D
	{
	public:

		Vector3d   Position;
		Vector3f   Scale;
		Rotation3D Rotation;

		Transform3D();

		Transform3D(const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		void ApplyToPoints(Vector3d* points, int count) const;

		void ApplyToPoint(Vector3d& point) const;

	};


	//################################################################ UTILS FOR 2D TYPES ###################################################


	template<typename T> inline Vector2<T> DivideSafe(const Vector2<T>& numeratorVector, const Vector2<T>& denominatorVector) 
	{
		return Vector2<T>(DivideSafe(numeratorVector.X, denominatorVector.X), DivideSafe(numeratorVector.Y, denominatorVector.Y));
	}

	template<typename T> inline Vector2<T> DivideSafe(const Vector2<T>& numeratorVector, T denominator)
	{
		return Vector2<T>(DivideSafe(numeratorVector.X, denominator), DivideSafe(numeratorVector.Y, denominator));
	}

	template<typename T> inline Vector2<T> InterpolateRaw(const Vector2<T>& startVector, const Vector2<T>& endVector, T alpha) 
	{
		return startVector + (endVector - startVector) * alpha;
	}

	inline Rotation2D Interpolate(const Rotation2D& startRotation, const Rotation2D& endRotation, float alpha) 
	{
		const Vector2f& xAxis = endRotation.GetXAxis();
		const Vector2f& previousXAxis = startRotation.GetXAxis();

		if (alpha > 1.0f) alpha = 1.0f;
		else if (alpha < 0.0f) alpha = 0.0f;

		// If rotation is more than 90° apart, return the target rotation directly
		if (xAxis.DotProduct(previousXAxis) < 0.0f)
			return Rotation2D(endRotation);

		Vector2f interpolatedRotX = InterpolateRaw(previousXAxis, xAxis, alpha);
		interpolatedRotX.Normalize();

		return Rotation2D(interpolatedRotX.X, interpolatedRotX.Y);
	}

	template<typename T> inline Vector2<T> RotatePointRaw(const Vector2<T>& xAxis, const Vector2<T>& point) 
	{
		return { xAxis.X * point.X - xAxis.Y * point.Y,
				 xAxis.Y * point.X + xAxis.X * point.Y };
	}


	//################################################################ UTILS FOR 3D TYPES ###################################################


	template<typename T> inline Vector3<T> DivideSafe(const Vector3<T>& numeratorVector, const Vector3<T>& denominatorVector)
	{
		return Vector3<T>(DivideSafe(numeratorVector.X, denominatorVector.X), DivideSafe(numeratorVector.Y, denominatorVector.Y), DivideSafe(numeratorVector.Z, denominatorVector.Z));
	}

	template<typename T> inline Vector3<T> DivideSafe(const Vector3<T>& numeratorVector, T denominator)
	{
		return Vector3<T>(DivideSafe(numeratorVector.X, denominator), DivideSafe(numeratorVector.Y, denominator), DivideSafe(numeratorVector.Z, denominator));
	}

	template<typename T> inline Vector3<T> InterpolateRaw(const Vector3<T>& startVector, const Vector3<T>& endVector, T alpha)
	{
		return startVector + (endVector - startVector) * alpha;
	}

	inline Rotation3D Interpolate(const Rotation3D& prevRotation, const Rotation3D& rotation, float interpolationAlpha) 
	{
		const Vector3f& xAxis = rotation.GetXAxis();
		const Vector3f& yAxis = rotation.GetYAxis();
		const Vector3f& previousXAxis = prevRotation.GetXAxis();
		const Vector3f& previousYAxis = prevRotation.GetYAxis();

		if (interpolationAlpha > 1.0f) interpolationAlpha = 1.0f;
		if (interpolationAlpha < 0.0f) interpolationAlpha = 0.0f;

		// 90+ degrees is more than enough for interpolation to not make much sense
		if ((xAxis.DotProduct(previousXAxis) < 0.0f) || (yAxis.DotProduct(previousYAxis) < 0.0f))
			return Rotation3D(rotation);

		const Vector3f interpolatedRotX = InterpolateRaw(previousXAxis, xAxis, interpolationAlpha);
		const Vector3f interpolatedRotY = InterpolateRaw(previousYAxis, yAxis, interpolationAlpha);

		return Rotation3D(interpolatedRotX, interpolatedRotY);
	}




}
