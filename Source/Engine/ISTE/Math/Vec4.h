#pragma once
//STD
#include <cstring>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4201)
namespace CU
{
	template <class T>
	class Vec4
	{

	public:

		union {
			T myValue[4] = { 0 };
			struct {
				T x, y, z, w;
			};
			struct {
				T r, g, b, a;
			};
		};

		//constructors
		Vec4() :
			x(T()),
			y(T()),
			z(T()),
			w(T())
		{}
		Vec4(const T& aX, const T& aY, const T& aZ, const T& aW) :
			x(T(aX)),
			y(T(aY)),
			z(T(aZ)),
			w(T(aW))
		{}
		Vec4(const Vec4<T>& aVector4)
		{
			memcpy(this, &aVector4, sizeof(Vec4<T>));
		}

		//destructor
		~Vec4() = default;

		//assignment operator 
		Vec4<T>& operator=(const Vec4<T>& aVector4)
		{
			memcpy(this, &aVector4, sizeof(Vec4<T>));
			return *this;
		}

		//Vector4 Magnitude SQUARED
		T LengthSqr() const
		{ 
			return (x * x + y * y + z * z + w * w); 
		}

		//Vectors Magnitude
		T Length() const
		{ 
			return sqrt(x * x + y * y + z * z + w * w);
		}

		//normalize the current vector
		void Normalize()
		{
			if (x == 0 && y == 0 && z == 0 && w == 0)
				return;

			T length = sqrt(x * x + y * y + z * z + w * w);
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}

		//get a normalized version of the vector
		Vec4<T> GetNormalized() const
		{
			if (x == 0 && y == 0 && z == 0 && w == 0)
				return Vec4<T>();

			Vec4<T> tempVector(x, y, z, w);
			T length = sqrt(x * x + y * y + z * z + w * w);
			return (tempVector / length);
		}

		//multiplacations
		T Dot(const Vec4<T>& aRHSVector4) const
		{  
			return x * aRHSVector4.x + y * aRHSVector4.y + z * aRHSVector4.z + w * aRHSVector4.w;
		}

		static Vec4<T> Lerp(Vec4<T> aFrom, Vec4<T> aTo, T aBlend) {
			return aFrom + ((aFrom - aTo) * aBlend);
		}

		//set 
		void Set(const T& aX, const T& aY, const T& aZ, const T& aW) const
		{
			x = aX;
			y = aY;
			z = aZ;
			w = aW;
		}

		friend std::ostream& operator<<(std::ostream& aOut, const Vec4<T>& aVec)
		{
			return aOut << "{ X: " << aVec.x << " Y: " << aVec.y << " Z: " << aVec.z << " W: " << aVec.w << " }";
		}

	};
	typedef Vec4<float> Vec4f;
	typedef Vec4<unsigned int> Vec4Ui;
	typedef Vec4<int> Vec4i;

	template <class T>
	Vec4<T> operator+(const Vec4<T>& aVector0, const Vec4<T>& aVector1)
	{  
		return {
			aVector0.x + aVector1.x,
			aVector0.y + aVector1.y,
			aVector0.z + aVector1.z,
			aVector0.w + aVector1.w 
		};
	}

	template <class T>
	Vec4<T> operator-(const Vec4<T>& aVector0, const Vec4<T>& aVector1)
	{ 
		return{
			aVector0.x - aVector1.x,
			aVector0.y - aVector1.y,
			aVector0.z - aVector1.z,
			aVector0.w - aVector1.w 
		};
	}

	template <class T>
	Vec4<T> operator*(const Vec4<T>& aLVector, const Vec4<T>& aRVector)
	{
		return {
			aLVector.x * aRVector.x,
			aLVector.y * aRVector.y,
			aLVector.z * aRVector.z,
			aLVector.w * aRVector.w
		};
	}

	template <class T>
	Vec4<T> operator*(const Vec4<T>& aVector, const T& aScalar)
	{ 
		return {
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar,
			aVector.w * aScalar
		};
	}

	template <class T>
	Vec4<T> operator*(const T& aScalar, const Vec4<T>& aVector)
	{ 
		return  aVector * aScalar;
	}


	template <class T>
	Vec4<T> operator/(const Vec4<T>& aVector, T& aScalar)
	{
		return {
			aVector.x / aScalar,
			aVector.y / aScalar,
			aVector.z / aScalar,
			aVector.w / aScalar
		};
	}

	template <class T>
	Vec4<T> operator/(const Vec4<T>& aVector, const T& aScalar)
	{
		return {
			aVector.x / aScalar,
			aVector.y / aScalar,
			aVector.z / aScalar,
			aVector.w / aScalar
		};
	}

	template <class T>
	void operator+=(Vec4<T>& aVector0, const Vec4<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}


	template <class T>
	void operator-=(Vec4<T>& aVector0, const Vec4<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}


	template <class T>
	void operator*=(Vec4<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}


	template <class T>
	void operator/=(Vec4<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	void operator/=(Vec4<T>& aVector, T& aScalar)
	{
		aVector = aVector / aScalar;
	}
};
#pragma warning(pop)