#pragma once
//STD
#include <cstring>
#include <iostream> 
#include "Vec3.h" // to construct a vec2 of a vec3 using x and z

#pragma warning(push)
#pragma warning(disable: 4201)
namespace CU
{
	template <class T>
	class Vec2
	{
	public:
		union {
			T myValue[2] = {0};
			struct {
				T x, y;
			};
			struct {
				T r, g;
			};
		};

		//Default constructor. Sets everything to zero.
		Vec2() :
			x(T()),
			y(T())
		{}

		Vec2(const T& aX, const T& aY) :
			x(T(aX)),
			y(T(aY))
		{}
		Vec2(const Vec3<T>& aVector3) : // sorry george
			x(T(aVector3.x)),
			y(T(aVector3.z))
		{}

		Vec2(const Vec2<T>& aVector2) 
		{
			memcpy(this, &aVector2, sizeof(Vec2<T>));
		}
		//destructor
		~Vec2() = default;

		//assignment operator 
		Vec2<T>& operator=(const Vec2<T>& aVector2)
		{
			memcpy(this, &aVector2, sizeof(Vec2<T>));
			return *this;
		}

		//Vector2 Magnitude SQUARED
		T LengthSqr() const
		{
			return (x * x + y * y);
		}

		//Vectors Magnitude
		T Length() const
		{
			return sqrt(x * x + y * y);
		}

		//normalize the current vector
		void Normalize()
		{
			if (x == 0 && y == 0)
				return;

			T length = sqrt(x * x + y * y);
			x /= length;
			y /= length;
		}

		//get a normalized version of the vector
		Vec2<T> GetNormalized() const
		{
			if (x == 0 && y == 0)
				return Vec2<T>();

			Vec2<T> tempVector(x, y);
			T length = sqrt(x * x + y * y);
			return (tempVector / length);
		}

		//multiplacations
		T Dot(const Vec2<T>& aRHSVector2) const
		{
			return x * aRHSVector2.x + y * aRHSVector2.y;
		}
	
		static Vec2<T> Lerp(Vec2<T> aFrom, Vec2<T> aTo, T aBlend) {
			return aFrom + ((aFrom - aTo) * aBlend);
		}

		//set
		void Set(const T& aX, const T& aY)
		{
			x = aX;
			y = aY;
		}
		
		friend std::ostream& operator<<(std::ostream& aOut, const Vec2<T>& aVec) 
		{
			return aOut << "{ X: " << aVec.x << " Y: " << aVec.y << " }";
		}

	};
	typedef Vec2<float> Vec2f;
	typedef Vec2<unsigned int> Vec2Ui;
	typedef Vec2<int> Vec2i;

	template <class T>
	Vec2<T> operator+(const Vec2<T>& aVector0, const Vec2<T>& aVector1)
	{
		return { aVector0.x + aVector1.x, aVector0.y + aVector1.y };
	}

	template <class T>
	Vec2<T> operator-(const Vec2<T>& aVector0, const Vec2<T>& aVector1)
	{
		return { aVector0.x - aVector1.x, aVector0.y - aVector1.y };
	}

	template <class T>
	Vec2<T> operator*(const Vec2<T>& aLVec, const Vec2<T>& aRVec)
	{
		return { aLVec.x * aRVec.x, aLVec.y * aRVec.y };
	}

	template <class T>
	Vec2<T> operator*(const Vec2<T>& aVector, const T& aScalar)
	{
		return { aVector.x * aScalar, aVector.y * aScalar };
	}

	template <class T>
	Vec2<T> operator*(const T& aScalar, const Vec2<T>& aVector)
	{
		return aVector * aScalar;
	}

	template <class T>
	Vec2<T> operator/(const Vec2<T>& aVector, const T& aScalar)
	{
		return { aVector.x / aScalar, aVector.y / aScalar };
	}

	template <class T>
	void operator+=(Vec2<T>& aVector0, const Vec2<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vec2<T>& aVector0, const Vec2<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vec2<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vec2<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	// addedd 2022-10-12 // Mathias
	template <class T>
	inline Vec2<T> operator-(const Vec2<T>& aVector)
	{
		return Vec2<T>(-aVector.x, -aVector.y);
	}


};
#pragma warning(pop)