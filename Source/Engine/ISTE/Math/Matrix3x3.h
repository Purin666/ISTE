#pragma once
#include "Matrix4x4.h"

//CU
#include "Vec3.h"
#include "Vec4.h"

//STD
#include <cstring>
#include <array>


#pragma warning(push)
#pragma warning(disable: 4201)
namespace CU
{
	template <class T>
	class Matrix4x4;

	template <class T>
	class Matrix3x3
	{
	private:
		union
		{
			T mySingleArray[9] = {};
			struct {T myDoubleArray[3][3]; };
			struct { Vec3<T> myRows[3]; };
		};
	public:
		Matrix3x3<T>();
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

		Matrix3x3<T>&	operator=	(const Matrix3x3<T>& aRhsMatrix);
		bool			operator==	(const Matrix3x3<T>& aRhsMatrix);
		bool			operator!=	(const Matrix3x3<T>& aRhsMatrix);

		T&				operator()	(const int aRow, const int aColumn);
		const T&		operator()	(const int aRow, const int aColumn) const; 
		inline Vec3<T>& GetRow		(const size_t aRow); 
		inline Vec3<T>& GetRow		(const size_t aRow) const;

		Matrix3x3<T>	Transpose	(const Matrix3x3<T>& aMatrixToTranspose); 

		Matrix3x3<T>	operator+	(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>	operator-	(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>	operator*	(const Matrix3x3<T>& aMatrix) const;
		Matrix3x3<T>	operator*	(const T&			 aScalar) const;
		Vec3<T>			operator*	(const Vec3<T>&		 aVector) const;
		Matrix3x3<T>&	operator+=	(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>&	operator-=	(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>&	operator*=	(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>&	operator*=	(const T&			 aScalar); 

		/*	@brief			Creates a XYZ rotation matrix out of euler angles. 
			@param	aVec:	The euler angles to rotate by. The euler angles is assumed to be in radians.								*/
		static Matrix3x3<T> CreateRotationMatrix(Vec3<T> aVec);

		/*	@brief				Creates a XYZ rotation matrix out of euler angles.
			@param	aX, aY, aZ:	The euler angles to rotate by. The euler angles is assumed to be in radians.							*/
		static Matrix3x3<T> CreateRotationMatrix(T aX, T aY, T aZ);

		static Matrix3x3<T> CreateRotationAroundX(T anAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T anAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T anAngleInRadians);

		/*	@brief						Creates a rotation matrix around an inputted axis.
			@param	aVec:				The axis to revolve around.
			@param	aAngleInRadians:	an Angle in Radians.																			*/
		static Matrix3x3<T> CreateRotationAroundAxis(Vec3<T> aVec, T aAngleInRadians);
		 
		static Matrix3x3<T> CreateScaleMatrix(Vec3<T> aScale);
		static Matrix3x3<T> CreateScaleMatrix(T aX, T aY, T aZ);

		static Matrix3x3<T> CreateTranslationMatrix(Vec3<T> aScale); 
		static Matrix3x3<T> CreateTranslationMatrix(T aX, T aY, T aZ);


		/*	@brief						Decomposes a matrix in to scale rotation and translation
			@return	M3x3Decomposition:	The Decomposition of the matrix in a struct form with myScale, myRotation, and myTranslation	*/
		struct M3x3Decomposition {
			Vec3<T> myScale;
			Vec3<T> myRot;
			Vec3<T> myPos; 
		} DecomposeMatrix();


		/*	@brief						Decomposes the rotation out of a matrix
			@return	Vec3:				The rotation of the matrix																		*/
		CU::Vec3<T> DecomposeRotation();

		/*	@brief						Decomposes the scale out of a matrix
			@return	Vec3:				The scale of the matrix																			*/
		CU::Vec3<T> DecomposeScale();

		/*	@brief						Decomposes the translation out of a matrix
			@return	Vec3:				The translation of the matrix																	*/
		CU::Vec3<T> DecomposeTranslation();
	};
	typedef Matrix3x3<float> Matrix3x3f;
	
#pragma region Constructor 
	template<class T>
	CU::Matrix3x3<T>::Matrix3x3()
	{
		myDoubleArray[0][0] = 1;
		myDoubleArray[1][1] = 1;
		myDoubleArray[2][2] = 1;
	}
	
	template<class T>
	CU::Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		memcpy(this, &aMatrix, sizeof(Matrix3x3<T>));
	}
	
	template<class T>
	CU::Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		myDoubleArray[0][0] = aMatrix(1, 1);
		myDoubleArray[0][1] = aMatrix(1, 2);
		myDoubleArray[0][2] = aMatrix(1, 3);
		myDoubleArray[1][0] = aMatrix(2, 1);
		myDoubleArray[1][1] = aMatrix(2, 2);
		myDoubleArray[1][2] = aMatrix(2, 3);
		myDoubleArray[2][0] = aMatrix(3, 1);
		myDoubleArray[2][1] = aMatrix(3, 2);
		myDoubleArray[2][2] = aMatrix(3, 3);
	}

#pragma endregion


	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aRhsMatrix)//
	{
		memcpy(this, &aRhsMatrix, sizeof(Matrix3x3<T>));
		return *this;
	}

	template<class T>
	bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aRhsMatrix)
	{
		return memcmp(mySingleArray, &aRhsMatrix, sizeof(Matrix3x3<T>)) == 0;
	}

	template<class T>
	bool Matrix3x3<T>::operator!=(const Matrix3x3<T>& aRhsMatrix)
	{
		return !(memcmp(mySingleArray, &aRhsMatrix, sizeof(Matrix3x3<T>)) == 0);
	}

	template<class T>
	T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)//
	{
		return myDoubleArray[aRow - 1][aColumn - 1];
	}
	template<class T>
	const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const//
	{
		return myDoubleArray[aRow - 1][aColumn - 1];
	}

	template<class T>
	Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> tempMatrix = aMatrixToTranspose;
		
		tempMatrix.myDoubleArray[1][0] = aMatrixToTranspose.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[2][0] = aMatrixToTranspose.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[2][1] = aMatrixToTranspose.myDoubleArray[1][2];

		tempMatrix.myDoubleArray[0][1] = aMatrixToTranspose.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[0][2] = aMatrixToTranspose.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[1][2] = aMatrixToTranspose.myDoubleArray[2][1];

		return tempMatrix;
	}

	template<class T>
	inline Vec3<T>& Matrix3x3<T>::GetRow(const size_t aRow)
	{
		return myRows[aRow - 1];
	};

	template<class T>
	inline Vec3<T>& Matrix3x3<T>::GetRow(const size_t aRow) const
	{
		return myRows[aRow - 1];
	};

	 
	template<class T>
	static Matrix3x3<T> Matrix3x3<T>::CreateRotationMatrix(Vec3<T> aVec) {
		return CreateRotationMatrix(aVec.x, aVec.y, aVec.z);
	}

	template<class T>
	static Matrix3x3<T> Matrix3x3<T>::CreateRotationMatrix(T aX, T aY, T aZ) {
		Matrix3x3<T> m;
		T cX = cosf(aX);
		T cY = cosf(aY);
		T cZ = cosf(aZ);
		T sX = sinf(aX);
		T sY = sinf(aY);
		T sZ = sinf(aZ);

		m.myDoubleArray[0][0] = cY * cZ;
		m.myDoubleArray[0][1] = cY * sZ;
		m.myDoubleArray[0][2] = -sY;

		m.myDoubleArray[1][0] = (sX * cZ) * sY - (cX * sZ);
		m.myDoubleArray[1][1] = (sX * sZ) * sY + (cX * cZ);
		m.myDoubleArray[1][2] = sX * cY;

		m.myDoubleArray[2][0] = (cX * cZ) * sY + (sX * sZ);
		m.myDoubleArray[2][1] = (cX * sZ) * sY - (sX * cZ);
		m.myDoubleArray[2][2] = cX * cY;
		return m;
	}

	template<class T>
	static Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> tempRotationMatrix;
		tempRotationMatrix.myDoubleArray[0][0] = (T)1;
		tempRotationMatrix.myDoubleArray[1][1] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][2] = (T)sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][1] = (T)-sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][2] = (T)cos(aAngleInRadians);

		return tempRotationMatrix;
	}

	template<class T>
	static Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> tempRotationMatrix;
		tempRotationMatrix.myDoubleArray[0][0] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[0][2] = (T)-sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][1] = (T)1;
		tempRotationMatrix.myDoubleArray[2][0] = (T)sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][2] = (T)cos(aAngleInRadians);

		return tempRotationMatrix;
	}

	template<class T>
	static Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> tempRotationMatrix;
		tempRotationMatrix.myDoubleArray[0][0] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[0][1] = (T)sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][0] = (T)-sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][1] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][2] = (T)1;

		return tempRotationMatrix;
	}

	template<class T>
	static Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundAxis(Vec3<T> aVec, T aAngleInRadians)
	{
		Matrix3x3<T> I;
		//cross matrix
		Matrix3x3<T> K;
		K(1, 1) = 0;
		K(1, 2) = aVec.z;
		K(1, 3) = -aVec.y;

		K(2, 1) = -aVec.z;
		K(2, 2) = 0;
		K(2, 3) = aVec.x;

		K(3, 1) = aVec.y;
		K(3, 2) = -aVec.x;
		K(3, 3) = 0;
		K(4, 4) = 0;

		return (I + (K * (float)sin(aAngleInRadians)) + (K * K * (1 - (float)cos(aAngleInRadians))));
	}


	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aRightMatrix) const 
	{
		Matrix3x3<T> tempMatrix = *this;

		tempMatrix.myDoubleArray[0][0] += aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[0][1] += aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[0][2] += aRightMatrix.myDoubleArray[0][2];

		tempMatrix.myDoubleArray[1][0] += aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[1][1] += aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[1][2] += aRightMatrix.myDoubleArray[1][2];

		tempMatrix.myDoubleArray[2][0] += aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[2][1] += aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[2][2] += aRightMatrix.myDoubleArray[2][2];

		return tempMatrix;
	}
	
	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aRightMatrix) const
	{
		Matrix3x3<T> tempMatrix = *this;

		tempMatrix.myDoubleArray[0][0] -= aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[0][1] -= aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[0][2] -= aRightMatrix.myDoubleArray[0][2];

		tempMatrix.myDoubleArray[1][0] -= aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[1][1] -= aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[1][2] -= aRightMatrix.myDoubleArray[1][2];

		tempMatrix.myDoubleArray[2][0] -= aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[2][1] -= aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[2][2] -= aRightMatrix.myDoubleArray[2][2];

		return tempMatrix;
	}
	
	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aRightMatrix) const 
	{
		Matrix3x3<T> tempMatrix;
		tempMatrix.myDoubleArray[0][0] = 0;
		tempMatrix.myDoubleArray[1][1] = 0;
		tempMatrix.myDoubleArray[2][2] = 0;

		//ROW ONE

		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][0];

		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][1];

		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][2];


		//ROW TWO

		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][0];

		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][1];

		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][2];


		//ROW THREE

		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][0];

		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][1];

		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][2];


		return tempMatrix;
	}
	
	template<class T>
	Matrix3x3<T> Matrix3x3<T>::operator*(const T& aRightScalar) const 
	{
		Matrix3x3<T> tempMatrix = *this;

		tempMatrix.myDoubleArray[0][0] *= aRightScalar;
		tempMatrix.myDoubleArray[0][1] *= aRightScalar;
		tempMatrix.myDoubleArray[0][2] *= aRightScalar;

		tempMatrix.myDoubleArray[1][0] *= aRightScalar;
		tempMatrix.myDoubleArray[1][1] *= aRightScalar;
		tempMatrix.myDoubleArray[1][2] *= aRightScalar;

		tempMatrix.myDoubleArray[2][0] *= aRightScalar;
		tempMatrix.myDoubleArray[2][1] *= aRightScalar;
		tempMatrix.myDoubleArray[2][2] *= aRightScalar;

		return tempMatrix;
	}

	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aRhsMatrix)
	{
		*this = *this + aRhsMatrix;
		return *this;
	}

	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aRhsMatrix)
	{
		*this = *this - aRhsMatrix;
		return *this;
	}
	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aRhsMatrix)
	{
		*this = *this * aRhsMatrix;

		return *this;
	}

	template<class T>
	Matrix3x3<T>& Matrix3x3<T>::operator*=(const T& aScalar)
	{
		*this = *this * aScalar;

		return *this;
	}





	template<class T>
	Vec3<T> operator*(const Vec3<T>& aLhsVector, const Matrix3x3<T>& aRhsMatrix)
	{
		Matrix3x3<T> tempMatrix = aRhsMatrix;

		tempMatrix(1, 1) *= aLhsVector.x;
		tempMatrix(1, 2) *= aLhsVector.x;
		tempMatrix(1, 3) *= aLhsVector.x;

		tempMatrix(2, 1) *= aLhsVector.y;
		tempMatrix(2, 2) *= aLhsVector.y;
		tempMatrix(2, 3) *= aLhsVector.y;

		tempMatrix(3, 1) *= aLhsVector.z;
		tempMatrix(3, 2) *= aLhsVector.z;
		tempMatrix(3, 3) *= aLhsVector.z;

		return Vec3<T>(
			tempMatrix(1, 1) + tempMatrix(2, 1) + tempMatrix(3, 1),
			tempMatrix(1, 2) + tempMatrix(2, 2) + tempMatrix(3, 2),
			tempMatrix(1, 3) + tempMatrix(2, 3) + tempMatrix(3, 3));
	}
	

	template<class T>
	Matrix3x3<T> operator*(const Matrix3x3<T>& aLhsMatrix, const T aScalar)
	{
		return aScalar * aLhsMatrix;
	}
};
#pragma warning(pop)