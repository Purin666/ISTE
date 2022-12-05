#pragma once 

#include "Vec3.h"
#include "Matrix4x4.h"
#include "Matrix3x3.h"
#include "Math.h" 
#include <algorithm> 
#include <cmath>
/*
* Quaternion.h			Created and last edited by George 07-26
* -------------------------------------------------------------
* TODO
* 
*	-	should be looked at a second time atleast
*	-	should improve readability
* -------------------------------------------------------------
* 
* Following xyz rotation convention 
* all constructors 
* 
* Follows the left hand convention so it mimics our matrix rotations so far. 
*	- this was done by negating the axis-angle and euler constructors
* 
* ALSO, this quaternion is not really a pure rotation quaternion
* because i have to *-1.f w to make it mimic how it should rotate with the matrices that we use
* 
* This should only be used for in engine use, for rotation of in game enteties, use euler
*/

#pragma warning(push)
#pragma warning(disable: 4201)
namespace CU
{

	template<typename T>
	class Quaternion
	{
	public:
		union
		{
			T myValue[4] = { 0 };
			struct {
				T w, x, y, z;
			};
			struct
			{
				T myRealPart;
				Vec3<T> myImaginaryPart;
			};
		};

#pragma region "Constructors"
		~Quaternion() {};
		Quaternion()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 1;
		}
		Quaternion(const Quaternion<T>& aLeft)
		{
			memcpy(this, &aLeft, sizeof(Quaternion<T>));
		};

		Quaternion(Matrix3x3<T> aRM)
		{
			//https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/ 
			//the (std::max) is because somewhere somehow some .h file is undefening the NOMINMAX 
			//and doesnt redeffen it, and because of that it freaks out and screams error C2059
			w = sqrt((std::max)((T)0, (T)1 + aRM(1, 1) + aRM(2, 2) + aRM(3, 3))) / 2.f;
			x = sqrt((std::max)((T)0, (T)1 + aRM(1, 1) - aRM(2, 2) - aRM(3, 3))) / 2.f;
			y = sqrt((std::max)((T)0, (T)1 - aRM(1, 1) + aRM(2, 2) - aRM(3, 3))) / 2.f;
			z = sqrt((std::max)((T)0, (T)1 - aRM(1, 1) - aRM(2, 2) + aRM(3, 3))) / 2.f;

			x = std::copysign(x, aRM(2, 3) - aRM(3, 2));
			y = std::copysign(y, aRM(3, 1) - aRM(1, 3));
			z = std::copysign(z, aRM(1, 2) - aRM(2, 1)); 
		}
		Quaternion(Matrix4x4<T> aRM)
		{
			//https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/ 
			//the (std::max) is because somewhere somehow some .h file is undefening the NOMINMAX 
			//and doesnt redeffen it, and because of that it freaks out and screams error C2059
			w = sqrt((std::max)((T)0, (T)1 + aRM(1, 1) + aRM(2, 2) + aRM(3, 3))) / 2.f;
			x = sqrt((std::max)((T)0, (T)1 + aRM(1, 1) - aRM(2, 2) - aRM(3, 3))) / 2.f;
			y = sqrt((std::max)((T)0, (T)1 - aRM(1, 1) + aRM(2, 2) - aRM(3, 3))) / 2.f;
			z = sqrt((std::max)((T)0, (T)1 - aRM(1, 1) - aRM(2, 2) + aRM(3, 3))) / 2.f;

			x = std::copysign(x, aRM(2, 3) - aRM(3, 2));
			y = std::copysign(y, aRM(3, 1) - aRM(1, 3));
			z = std::copysign(z, aRM(1, 2) - aRM(2, 1));
		} 
		Quaternion(CU::Vec3<T> aVector, float aAngleInRadians)
		{
			//https://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
			
			myImaginaryPart = aVector * sinf(aAngleInRadians / 2.f);
			myRealPart = cosf(-aAngleInRadians / 2.f);
		} 
		Quaternion(Vec3<T> aPitchYawRoll)
		{
			//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_angles_to_quaternion_conversion
			//tested and made to fit xyz through https://www.andre-gaschler.com/rotationconverter/
			T cP = cos(-aPitchYawRoll.x * (T)0.5);
			T cY = cos(-aPitchYawRoll.y * (T)0.5);
			T cR = cos(-aPitchYawRoll.z * (T)0.5);
			T sP = sin(-aPitchYawRoll.x * (T)0.5);
			T sY = sin(-aPitchYawRoll.y * (T)0.5);
			T sR = sin(-aPitchYawRoll.z * (T)0.5);
			
			x =   sP* cY* cR + cP * sY * sR;
			y =   cP* sY* cR - sP * cY * sR;
			z =   cP* cY* sR + sP * sY * cR;
			w = -(cP* cY* cR - sP * sY * sR);
		} 
		Quaternion(T aX, T aY, T aZ, T aW)
		{
			x = aX;
			y = aY;
			z = aZ;
			w = aW;
		}
#pragma endregion

#pragma region "Length, Dot, Conjugate, Normalize and Inverse"

		inline T Length() {
			return sqrt(x * x + y * y + z * z + w * w);
		}
		inline T LengthSqr() {
			return x * x + y * y + z * z + w * w;
		}

		inline T Dot(Quaternion<T> aLeft)
		{
			return x * aLeft.x + y * aLeft.y + z * aLeft.z + w * aLeft.w;
		}

		inline void Conjugate()
		{
			myImaginaryPart *= -1;
		}
		inline Quaternion<T> GetConjugate()
		{
			Quaternion<T> temp;
			memcpy(&temp, &temp, sizeof(Quaternion<T>));
			temp.myImaginaryPart *= -1;
			return temp;
		}

		inline void Inverse()
		{
			//(q*)/(||q||^2)
			myImaginaryPart *= -1;
			T lengthSqr = +x * x + y * y + z * z + w * w;
			x /= lengthSqr;
			y /= lengthSqr;
			z /= lengthSqr;
			w /= lengthSqr;
		}
		inline Quaternion<T> GetInverse()
		{
			Quaternion<T> temp;
			memcpy(&temp, &temp, sizeof(Quaternion<T>));

			temp.myImaginaryPart *= -1;
			T lengthSqr = + x * x + y * y + z * z + w * w;
			temp.x /= lengthSqr;
			temp.y /= lengthSqr;
			temp.z /= lengthSqr;
			temp.w /= lengthSqr;

			return temp;
		}

		inline void Normalize()
		{
			//(q)/(||q||) 
			T lengthSqr = +sqrt(x * x + y * y + z * z + w * w);
			x /= lengthSqr;
			y /= lengthSqr;
			z /= lengthSqr;
			w /= lengthSqr;
		}
		inline Quaternion<T> GetNormalized()
		{
			Quaternion<T> temp;
			memcpy(&temp, this, sizeof(Quaternion<T>));

			T lengthSqr = sqrt(x * x + y * y + z * z + w * w);
			temp.x /= lengthSqr;
			temp.y /= lengthSqr;
			temp.z /= lengthSqr;
			temp.w /= lengthSqr;

			return temp;
		}
#pragma endregion

		
		Matrix3x3<T> GetRotationMatrix3X3()
		{
			//LinAlg 284
			Matrix4x4<T> temp;
			temp(1, 1) = (1 - 2*(y*y + z*z));
			temp(2, 1) = (2 *   (x*y - w*z));
			temp(3, 1) = (2 *   (x*z + w*y));
			
			temp(1, 2) = (2 *   (x*y + w*z));
			temp(2, 2) = (1 - 2*(x*x + z*z));
			temp(3, 2) = (2 *   (y*z - w*x));
			
			temp(1, 3) = (2 * (x * z - w * y));
			temp(2, 3) = (2 * (y * z + w * x));
			temp(3, 3) = (1 - 2*(x*x + y*y));
			//LinAlg 284	slightly modified

			return temp;
		}
		Matrix4x4<T> GetRotationMatrix4X4()
		{
			Matrix4x4<T> temp;
			temp(1, 1) = (1 - 2*(y*y + z*z));
			temp(2, 1) = (2 *   (x*y - w*z));
			temp(3, 1) = (2 *   (x*z + w*y));
			
			temp(1, 2) = (2 *   (x*y + w*z));
			temp(2, 2) = (1 - 2*(x*x + z*z));
			temp(3, 2) = (2 *   (y*z - w*x));
			
			temp(1, 3) = (2 * (x * z - w * y));
			temp(2, 3) = (2 * (y * z + w * x));
			temp(3, 3) = (1 - 2*(x*x + y*y));
			//LinAlg 284	slightly modified

			//temp = CU::Matrix4x4<T>::Transpose(temp);

			return temp;
		}

		Vec3<T> GetEulerInRadians() {

			Matrix3x3<T> m = GetRotationMatrix3X3();
			//m.Transpose(m);
			Vec3<T> eulerAngles;

			eulerAngles.y = asin(m(1,3));
			//clamps it
			if (eulerAngles.y >= 1)
				eulerAngles.y = 1;
			else if (eulerAngles.y <= -1)
				eulerAngles.y = -1;

			if (abs(m(1,3)) < 0.9999999) { 
				eulerAngles.x = atan2(-m(2,3), m(3,3));
				eulerAngles.z = atan2(-m(1,2), m(1,1));
			}
			else { 
				eulerAngles.x = atan2(m(3,2), m(2,2));
				eulerAngles.z = 0; 
			}

			return eulerAngles;
		}
		Vec3<T> GetEulerInDegrees() {
			return GetEulerInRadians() * ISTE::RadToDeg;
		}

		inline Vec3<T> RotateVector(const Vec3<T>& aOtherVec) {
			return {
				(w * w - myImaginaryPart.Dot(myImaginaryPart)) * aOtherVec +
				T(2) * myImaginaryPart * (myImaginaryPart.Dot(aOtherVec)) +
				T(2) * w * (myImaginaryPart.Cross(aOtherVec))
			};
		}
		inline Vec4<T> RotateVector(const Vec4<T>& aOtherVec) {

			Vec3<T> v = {
				(w * w - myImaginaryPart.Dot(myImaginaryPart)) * v +
				T(2) * myImaginaryPart * (myImaginaryPart.Dot(v)) +
				T(2) * w * (myImaginaryPart.Cross(v))
			};
			return { v.x, v.y, v.z, aOtherVec.w };
		}

		static Quaternion<T> NLerp(const Quaternion<T>& aLeft, const Quaternion<T>& aRight, float t)
		{
			Quaternion<T> lerp = {
				lerp.x = (1 - t) * aLeft.x + (t)*aRight.x,
				lerp.y = (1 - t) * aLeft.y + (t)*aRight.y,
				lerp.z = (1 - t) * aLeft.z + (t)*aRight.z,
				lerp.w = (1 - t) * aLeft.w + (t)*aRight.w
			};
			return lerp.GetNormalized();
		}
		static Quaternion<T> Slerp(Quaternion<T> aLeft, Quaternion<T> aRight, float t)
		{
			float theta = aLeft.Dot(aRight);
			
			if (theta < 0)
			{
				theta = -theta;
				aRight.x = -aRight.x;
				aRight.y = -aRight.y;
				aRight.z = -aRight.z;
				aRight.w = -aRight.w;
			}
			
			if (1 - theta > 0.01)
			{
				T angle = acos(theta);
				return (sin((1 - t) * angle) * aLeft + sin(t * angle) * aRight) / sin(angle);
			}
			else
				return NLerp(aLeft, aRight, t);
		}

		inline Vec3<T> GetRight()
		{
			return RotateVector({ 1,0,0 });
		}
		inline Vec3<T> GetUp()
		{
			return RotateVector({ 0,1,0 });
		}
		inline Vec3<T> GetForward()
		{
			return RotateVector({ 0,0,1 });
		} 

		template<typename T>
		Quaternion<T>& operator=(const Quaternion<T>& aLeft)
		{
			memcpy(this, &aLeft, sizeof(Quaternion<T>));
			return *this;
		}
		Quaternion<T>& operator=(Quaternion<T>& aLeft)
		{
			memcpy(this, &aLeft, sizeof(Quaternion<T>));
			return *this;
		}
	};

	typedef Quaternion<float> Quaternionf;
	typedef Quaternion<double> Quaterniond;
#pragma region "operator overloads" 
	template<typename T>
	bool operator==(const Quaternion<T>& aRight, Quaternion<T>& aLeft)
	{
		return memcmp(&aRight, &aLeft, sizeof(Quaternion<T>)) == 0;
	}
	template<typename T>
	bool operator!=(const Quaternion<T>& aRight, const Quaternion<T>& aLeft)
	{
		return memcmp(&aRight, &aLeft, sizeof(Quaternion<T>)) != 0;
	}

	template<typename T>
	Quaternion<T> operator*(const Quaternion<T>& aRight, const Quaternion<T>& aLeft)
	{
		//LinAlg booken p252
		return Quaternion<T>(
			aRight.w * aLeft.x + aRight.x * aLeft.w + aRight.y * aLeft.z - aRight.z * aLeft.y,
			aRight.w * aLeft.y + aRight.y * aLeft.w + aRight.z * aLeft.x - aRight.x * aLeft.z,
			aRight.w * aLeft.z + aRight.z * aLeft.w + aRight.x * aLeft.y - aRight.y * aLeft.x,
			aRight.w * aLeft.w - aRight.x * aLeft.x - aRight.y * aLeft.y - aRight.z * aLeft.z
			);
	}
	template<typename T>
	Quaternion<T> operator*=(Quaternion<T>& aRight, const Quaternion<T>& aLeft)
	{
		//LinAlg booken p252
		aRight = aRight * aLeft;
		return aRight;
	}


	template<typename T>
	Quaternion<T> operator*(Quaternion<T>& aQuat, const T& aScalar)
	{
		return {
			aQuat.x * aScalar,
			aQuat.y * aScalar,
			aQuat.z * aScalar,
			aQuat.w * aScalar
		};
	}
	template<typename T>
	Quaternion<T> operator*=(Quaternion<T>& aQuat, const T& aScalar)
	{
		return aQuat = aQuat * aScalar;
	}
	template<typename T>
	Quaternion<T> operator*(const T& aScalar, Quaternion<T>& aQuat)
	{
		return aQuat * aScalar;
	}


	template<typename T>
	Quaternion<T> operator/(Quaternion<T>& aQuat, const T& aScalar)
	{
		return {
			aQuat.x / aScalar,
			aQuat.y / aScalar,
			aQuat.z / aScalar,
			aQuat.w / aScalar
		};
	}
	template<typename T>
	Quaternion<T> operator/=(Quaternion<T>& aQuat, const T& aScalar)
	{
		return aQuat = aQuat / aScalar;
	}
	template<typename T>
	Quaternion<T> operator/(const T& aScalar, Quaternion<T>& aQuat)
	{
		return aQuat / aScalar;
	}


	template<typename T>
	Quaternion<T> operator+(Quaternion<T>& aLeft, Quaternion<T>& aRight)
	{
		return {
			aLeft.x + aRight.x,
			aLeft.y + aRight.y,
			aLeft.z + aRight.z,
			aLeft.w + aRight.w
		};
	}

#pragma endregion
}
#pragma warning(pop)