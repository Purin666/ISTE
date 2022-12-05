#pragma once
//STD
#include <cstring>
#include <iostream> 

#include "Vec4.h" // for operator vec3 =, +, += vec4 // Mathias 2022-10-17

#pragma warning(push)
#pragma warning(disable: 4201)
namespace CU
{
	template <class T>
	class Vec3
	{

	public:
		union {
			T myValue[3] = { 0 };
			struct { T x, y, z; };
			struct { T r, g, b; };
		};
		

		//constructors
		Vec3() :
			x(T()),
			y(T()),
			z(T())
		{}
		Vec3(const T& aX, const T& aY, const T& aZ) :
			x(aX),
			y(aY),
			z(aZ)
		{}
		Vec3(const Vec3<T>& aVector3)
		{
			memcpy(this, &aVector3, sizeof(Vec3<T>));
		}

		//assignment operator 
		Vec3<T>& operator=(const Vec3<T>& aVector3)
		{
			memcpy(this, &aVector3, sizeof(Vec3<T>));
			return *this;
		}
		Vec3<T>& operator=(const Vec4<T>& aVector4)
		{
			memcpy(this, &aVector4, sizeof(Vec3<T>));
			return *this;
		}

		//destructor
		~Vec3() = default;

		//Vector3 Length
		T LengthSqr() const
		{
			return (x * x + y * y + z * z);
		}

		T Length() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		//normalize
		void Normalize()
		{
			if (x == 0 && y == 0 && z == 0)
				return;

			T length = sqrt(x * x + y * y + z * z);
			x /= length;
			y /= length;
			z /= length;
		}

		Vec3<T> GetNormalized() const
		{
			if (x == 0 && y == 0 && z == 0)
				return Vec3<T>();

			Vec3<T> tempVector(x, y, z);
			T length = sqrt(x * x + y * y + z * z);
			return (tempVector / length);
		}

		T Dot(const Vec3<T>& aRHSVector3) const
		{
			return x * aRHSVector3.x + y * aRHSVector3.y + z * aRHSVector3.z;
		}

		Vec3<T> Cross(const Vec3<T>& aRHSVector3) const
		{
			return {
				y * aRHSVector3.z - aRHSVector3.y * z,
				z * aRHSVector3.x - aRHSVector3.z * x,
				x * aRHSVector3.y - aRHSVector3.x * y
				};
			
		}

		static Vec3<T> Lerp(Vec3<T> aFrom, Vec3<T> aTo, T aBlend) {
			return aFrom + ((aTo - aFrom) * aBlend);
		}

		void Set(const T& aX, const T& aY, const T& aZ) const
		{
			x = aX;
			y = aY;
			z = aZ;
		} 

		friend std::ostream& operator<<(std::ostream& aOut, const Vec3<T>& aVec)
		{
			return aOut << "{ X: " << aVec.x << " Y: " << aVec.y << " Z: " << aVec.z << " }";
		}
	};
	typedef Vec3<float> Vec3f;
	typedef Vec3<unsigned int> Vec3Ui;
	typedef Vec3<int> Vec3i;

	template <class T>
	bool operator==(const Vec3<T>& aVector0, const Vec3<T>& aVector1)
	{
		return memcmp(&aVector0, &aVector1, sizeof(Vec3<T>)) == 0;
	}

	template <class T>
	bool operator!=(const Vec3<T>& aVector0, const Vec3<T>& aVector1)
	{
		return memcmp(&aVector0, &aVector1, sizeof(Vec3<T>)) != 0;
	}

	template <class T>
	Vec3<T> operator+(const Vec3<T>& aVector0, const Vec3<T>& aVector1)
	{
		return { aVector0.x + aVector1.x ,  aVector0.y + aVector1.y  , aVector0.z + aVector1.z };
	}

	template <class T>
	Vec3<T> operator+(const Vec3<T>& aVector0, const Vec4<T>& aVector1)
	{
		return { aVector0.x + aVector1.x ,  aVector0.y + aVector1.y  , aVector0.z + aVector1.z };
	}

	template <class T>
	Vec3<T> operator-(const Vec3<T>& aVector0, const Vec3<T>& aVector1)
	{
		return { aVector0.x - aVector1.x , aVector0.y - aVector1.y , aVector0.z - aVector1.z };
	}

	template <class T>
	Vec3<T> operator*(const Vec3<T>& aVector0, const Vec3<float>& aVector1) 
	{
		return { aVector0.x * aVector1.x , aVector0.y * aVector1.y , aVector0.z * aVector1.z };
	}

	template <class T>
	Vec3<T> operator*(const Vec3<T>& aVector, const T& aScalar)
	{
		return { aVector.x * aScalar , aVector.y * aScalar , aVector.z * aScalar };
	}


	template <class T>
	Vec3<T> operator*(const T& aScalar, const Vec3<T>& aVector)
	{
		return aVector * aScalar;
	} 

	template <class T>
	Vec3<T> operator/(const Vec3<T>& aVector, const T& aScalar)
	{
		return { aVector.x / aScalar, aVector.y / aScalar,  aVector.z / aScalar };
	}

	// addedd 2022-10-08 // Mathias
	template <class T>
	Vec3<T> operator/(const Vec3<T>& aFirst, const Vec3<T>& aSecond)
	{
		return { aFirst.x / aSecond.x, aFirst.y / aSecond.y,  aFirst.z / aSecond.z };
	}

	template <class T>
	void operator+=(Vec3<T>& aVector0, const Vec3<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}
	template <class T>
	void operator+=(Vec3<T>& aVector0, const Vec4<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vec3<T>& aVector0, const Vec3<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vec3<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vec3<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	// addedd 2022-10-12 // Mathias
	template <class T>
	inline Vec3<T> operator-(const Vec3<T>& aVector)
	{
		return Vec3<T>(-aVector.x, -aVector.y, -aVector.z);
	}



};
#pragma warning(pop)