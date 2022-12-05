#pragma once 
#include "Matrix3x3.h"

//CU
#include "Vec4.h" 
#include "Vec2.h" 
#include "Math.h"

//STD
#include <array>
#include <cstring>

/*   
	Creates	Scale, rot, transl, 

	SetScale
*/



#pragma warning(push)
#pragma warning(disable: 4201)
namespace CU
{ 
	template <class T>
	class Matrix3x3; 
	template <class T>
	class Matrix4x4
	{
	public: // structs

	public:
		Matrix4x4<T>(); 
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		Matrix4x4<T>&	operator=	(const Matrix4x4<T>& aRhsMatrix);
		bool			operator==	(const Matrix4x4<T>& aRhsMatrix);
		bool			operator!=	(const Matrix4x4<T>& aRhsMatrix);

		T&				operator()	(const int aRow, const int aColumn);
		const T&		operator()	(const int aRow, const int aColumn) const;

		//
		inline Vec4<T>& GetRow		(const size_t aRow);
		inline Vec4<T>& GetRow		(const size_t aRow) const;

		inline static Matrix4x4<T>	Transpose	(const Matrix4x4<T>& aMatrixToTranspose);
		inline static Matrix4x4<T>	Inverse		(const Matrix4x4<T>& aMatrixToTranspose);
		inline static Matrix4x4<T>	FastInverse	(const Matrix4x4<T>& aMatrixToTranspose);

		Vec4<T>		GetRight			() const;
		Vec4<T>&	GetRight			();
		Vec3<T>		GetRightV3			() const;
		Vec3<T>&	GetRightV3			();
		Vec4<T>		GetUp				() const; 
		Vec4<T>&	GetUp				();
		Vec3<T>		GetUpV3				() const; 
		Vec3<T>&	GetUpV3				();
		Vec4<T>		GetForward			() const;
		Vec4<T>&	GetForward			();
		Vec3<T>		GetForwardV3		() const;
		Vec3<T>&	GetForwardV3		();
		Vec4<T>		GetTranslation		() const;
		Vec4<T>&	GetTranslation		();
		Vec3<T>		GetTranslationV3	() const;
		Vec3<T>&	GetTranslationV3	();

		Matrix4x4<T>	operator+	(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T>	operator-	(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T>	operator*	(const Matrix4x4<T>& aMatrix) const;
		Matrix4x4<T>	operator*	(const T& aScalar) const;

		Vec4<T>			operator*	(const Vec4<T>& aVector) const;

		Matrix4x4<T>&	operator+=	(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>&	operator-=	(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>&	operator*=	(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>&	operator*=	(const T& aScalar);
		
		/*	@brief			Creates a XYZ rotation matrix out of euler angles. 
			@param	aVec:	The euler angles to rotate by. The euler angles is assumed to be in radians.								*/
		static Matrix4x4<T> CreateRotationMatrix(Vec3<T> aVec);

		/*	@brief				Creates a XYZ rotation matrix out of euler angles.
			@param	aX, aY, aZ:	The euler angles to rotate by. The euler angles is assumed to be in radians.							*/
		static Matrix4x4<T> CreateRotationMatrix(T aX, T aY, T aZ);

		static Matrix4x4<T> CreateRotationAroundX(T anAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T anAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T anAngleInRadians);

		/*	@brief						Creates a rotation matrix around an inputted axis.
			@param	aVec:				The axis to revolve around.
			@param	aAngleInRadians:	an Angle in Radians.																			*/
		static Matrix4x4<T> CreateRotationAroundAxis(Vec4<T> aVec, T aAngleInRadians);
		 
		static Matrix4x4<T> CreateScaleMatrix(Vec3<T> aScale);
		static Matrix4x4<T> CreateScaleMatrix(T aX, T aY, T aZ);

		static Matrix4x4<T> CreateTranslationMatrix(Vec3<T> aScale);
		static Matrix4x4<T> CreateTranslationMatrix(T aX, T aY, T aZ);

		/*	@brief						Decomposes a matrix in to scale rotation and translation
			@return	M4x4Decomposition:	The Decomposition of the matrix in a struct form with myScale, myRotation, and myTranslation	*/
		void DecomposeMatrix(Vec3<T>& outRotation, Vec3<T>& outScale, Vec3<T>& outTranlation);

		/*	@brief						Decomposes the rotation out of a matrix
			@return	Vec3:				The rotation of the matrix																		*/
		CU::Vec3<T> DecomposeRotation(); 

		/*	@brief						Decomposes the scale out of a matrix
			@return	Vec3:				The scale of the matrix																			*/
		CU::Vec3<T> DecomposeScale(); 



		static Matrix4x4<T> CreateProjectionMatrix		(const float aHorizontalFovInDeg, const CU::Vec2Ui& aRes, const float aNearPlane, const float aFarPlane);
		static Matrix4x4<T> CreateOrthographicMatrix	(const CU::Vec2Ui & aRes, const float aNearPlane, const float aFarPlane);
		static Matrix4x4<T> CreateOrthographicMatrix	(const T aLeft, const T aRight, const T aBottom, const T aTop, const T aNear, const T aFar);

		inline void SetScale(const Vec3<T>& aVec3);

	private:
		union
		{
			T mySingleArray[16] = {};
			struct {
				T myDoubleArray[4][4];
			};
			struct
			{
				Vec4<T> myRows[4];
			};
			struct
			{
				Vec3<T> myRight;		float myRightW;
				Vec3<T> myUp;			float myUpW;
				Vec3<T> myForward;		float myForwardW;
				Vec3<T> myTranslation;	float myTranslationW;
			};
		};
	};
	typedef Matrix4x4<float> Matrix4x4f;  

	template<class T>
	Matrix4x4<T>::Matrix4x4()
	{
		myDoubleArray[0][0] = 1;
		myDoubleArray[1][1] = 1;
		myDoubleArray[2][2] = 1;
		myDoubleArray[3][3] = 1;
	}
	template<class T>
	Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		memcpy(this, &aMatrix, sizeof(Matrix4x4<T>));
	}

	template<class T>
	Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aRhsMatrix)
	{
		memcpy(this, &aRhsMatrix, sizeof(Matrix4x4<T>));
		return *this;
	}
	template<class T>
	bool  Matrix4x4<T>::operator==(const Matrix4x4<T>& aRightMatrix)
	{
		return memcmp(mySingleArray, &aRightMatrix, sizeof(Matrix4x4<T>)) == 0;
	}
	template<class T>
	bool  Matrix4x4<T>::operator!=(const Matrix4x4<T>& aRightMatrix)
	{
		return !(memcmp(mySingleArray, &aRightMatrix, sizeof(Matrix4x4<T>)) == 0);
	} 

	template<class T>
	T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		return myDoubleArray[aRow - 1][aColumn - 1];
	}
	template<class T>
	const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		return myDoubleArray[aRow - 1][aColumn - 1];
	}





	template<class T>
	inline Vec4<T>& Matrix4x4<T>::GetRow(const size_t aRow)
	{
		return myRows[aRow - 1];
	}
	template<class T>
	inline Vec4<T>& Matrix4x4<T>::GetRow(const size_t aRow) const
	{
		return myRows[aRow - 1];
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> tempMatrix = aMatrixToTranspose;

		tempMatrix.myDoubleArray[1][0] = aMatrixToTranspose.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[2][0] = aMatrixToTranspose.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[3][0] = aMatrixToTranspose.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[3][1] = aMatrixToTranspose.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[3][2] = aMatrixToTranspose.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[2][1] = aMatrixToTranspose.myDoubleArray[1][2];


		tempMatrix.myDoubleArray[0][1] = aMatrixToTranspose.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[0][2] = aMatrixToTranspose.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[0][3] = aMatrixToTranspose.myDoubleArray[3][0];
		tempMatrix.myDoubleArray[1][3] = aMatrixToTranspose.myDoubleArray[3][1];
		tempMatrix.myDoubleArray[2][3] = aMatrixToTranspose.myDoubleArray[3][2];
		tempMatrix.myDoubleArray[1][2] = aMatrixToTranspose.myDoubleArray[2][1];
		return tempMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::FastInverse(const Matrix4x4<T>& aTransform)
	{
		// could need a refactoring

		Matrix3x3 <T> tempMatrix = aTransform;
		tempMatrix = tempMatrix.Transpose(tempMatrix);

		Vec3<T> tempVector(-aTransform.myDoubleArray[3][0], -aTransform.myDoubleArray[3][1], -aTransform.myDoubleArray[3][2]);

		tempVector = tempVector * tempMatrix;
		Matrix4x4<T> temp4x4Matrix;
		//could be optimized 
		for (int i = 1; i <= 3; i++)
		{
			for (int j = 1; j <= 3; j++)
			{
				temp4x4Matrix(i, j) = tempMatrix(i, j);
			}
		}

		temp4x4Matrix(4, 1) = tempVector.x;
		temp4x4Matrix(4, 2) = tempVector.y;
		temp4x4Matrix(4, 3) = tempVector.z;

		return temp4x4Matrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Inverse(const Matrix4x4<T>& aM)
	{
		T inv[16];

		inv[0] =
			aM.mySingleArray[5] * aM.mySingleArray[10] * aM.mySingleArray[15] -
			aM.mySingleArray[5] * aM.mySingleArray[11] * aM.mySingleArray[14] -
			aM.mySingleArray[9] * aM.mySingleArray[6] * aM.mySingleArray[15] +
			aM.mySingleArray[9] * aM.mySingleArray[7] * aM.mySingleArray[14] +
			aM.mySingleArray[13] * aM.mySingleArray[6] * aM.mySingleArray[11] -
			aM.mySingleArray[13] * aM.mySingleArray[7] * aM.mySingleArray[10];

		inv[4] = -aM.mySingleArray[4] * aM.mySingleArray[10] * aM.mySingleArray[15] +
			aM.mySingleArray[4] * aM.mySingleArray[11] * aM.mySingleArray[14] +
			aM.mySingleArray[8] * aM.mySingleArray[6] * aM.mySingleArray[15] -
			aM.mySingleArray[8] * aM.mySingleArray[7] * aM.mySingleArray[14] -
			aM.mySingleArray[12] * aM.mySingleArray[6] * aM.mySingleArray[11] +
			aM.mySingleArray[12] * aM.mySingleArray[7] * aM.mySingleArray[10];

		inv[8] = aM.mySingleArray[4] * aM.mySingleArray[9] * aM.mySingleArray[15] -
			aM.mySingleArray[4] * aM.mySingleArray[11] * aM.mySingleArray[13] -
			aM.mySingleArray[8] * aM.mySingleArray[5] * aM.mySingleArray[15] +
			aM.mySingleArray[8] * aM.mySingleArray[7] * aM.mySingleArray[13] +
			aM.mySingleArray[12] * aM.mySingleArray[5] * aM.mySingleArray[11] -
			aM.mySingleArray[12] * aM.mySingleArray[7] * aM.mySingleArray[9];

		inv[12] = -aM.mySingleArray[4] * aM.mySingleArray[9] * aM.mySingleArray[14] +
			aM.mySingleArray[4] * aM.mySingleArray[10] * aM.mySingleArray[13] +
			aM.mySingleArray[8] * aM.mySingleArray[5] * aM.mySingleArray[14] -
			aM.mySingleArray[8] * aM.mySingleArray[6] * aM.mySingleArray[13] -
			aM.mySingleArray[12] * aM.mySingleArray[5] * aM.mySingleArray[10] +
			aM.mySingleArray[12] * aM.mySingleArray[6] * aM.mySingleArray[9];

		inv[1] = -aM.mySingleArray[1] * aM.mySingleArray[10] * aM.mySingleArray[15] +
			aM.mySingleArray[1] * aM.mySingleArray[11] * aM.mySingleArray[14] +
			aM.mySingleArray[9] * aM.mySingleArray[2] * aM.mySingleArray[15] -
			aM.mySingleArray[9] * aM.mySingleArray[3] * aM.mySingleArray[14] -
			aM.mySingleArray[13] * aM.mySingleArray[2] * aM.mySingleArray[11] +
			aM.mySingleArray[13] * aM.mySingleArray[3] * aM.mySingleArray[10];

		inv[5] = aM.mySingleArray[0] * aM.mySingleArray[10] * aM.mySingleArray[15] -
			aM.mySingleArray[0] * aM.mySingleArray[11] * aM.mySingleArray[14] -
			aM.mySingleArray[8] * aM.mySingleArray[2] * aM.mySingleArray[15] +
			aM.mySingleArray[8] * aM.mySingleArray[3] * aM.mySingleArray[14] +
			aM.mySingleArray[12] * aM.mySingleArray[2] * aM.mySingleArray[11] -
			aM.mySingleArray[12] * aM.mySingleArray[3] * aM.mySingleArray[10];

		inv[9] = -aM.mySingleArray[0] * aM.mySingleArray[9] * aM.mySingleArray[15] +
			aM.mySingleArray[0] * aM.mySingleArray[11] * aM.mySingleArray[13] +
			aM.mySingleArray[8] * aM.mySingleArray[1] * aM.mySingleArray[15] -
			aM.mySingleArray[8] * aM.mySingleArray[3] * aM.mySingleArray[13] -
			aM.mySingleArray[12] * aM.mySingleArray[1] * aM.mySingleArray[11] +
			aM.mySingleArray[12] * aM.mySingleArray[3] * aM.mySingleArray[9];

		inv[13] = aM.mySingleArray[0] * aM.mySingleArray[9] * aM.mySingleArray[14] -
			aM.mySingleArray[0] * aM.mySingleArray[10] * aM.mySingleArray[13] -
			aM.mySingleArray[8] * aM.mySingleArray[1] * aM.mySingleArray[14] +
			aM.mySingleArray[8] * aM.mySingleArray[2] * aM.mySingleArray[13] +
			aM.mySingleArray[12] * aM.mySingleArray[1] * aM.mySingleArray[10] -
			aM.mySingleArray[12] * aM.mySingleArray[2] * aM.mySingleArray[9];

		inv[2] = aM.mySingleArray[1] * aM.mySingleArray[6] * aM.mySingleArray[15] -
			aM.mySingleArray[1] * aM.mySingleArray[7] * aM.mySingleArray[14] -
			aM.mySingleArray[5] * aM.mySingleArray[2] * aM.mySingleArray[15] +
			aM.mySingleArray[5] * aM.mySingleArray[3] * aM.mySingleArray[14] +
			aM.mySingleArray[13] * aM.mySingleArray[2] * aM.mySingleArray[7] -
			aM.mySingleArray[13] * aM.mySingleArray[3] * aM.mySingleArray[6];

		inv[6] = -aM.mySingleArray[0] * aM.mySingleArray[6] * aM.mySingleArray[15] +
			aM.mySingleArray[0] * aM.mySingleArray[7] * aM.mySingleArray[14] +
			aM.mySingleArray[4] * aM.mySingleArray[2] * aM.mySingleArray[15] -
			aM.mySingleArray[4] * aM.mySingleArray[3] * aM.mySingleArray[14] -
			aM.mySingleArray[12] * aM.mySingleArray[2] * aM.mySingleArray[7] +
			aM.mySingleArray[12] * aM.mySingleArray[3] * aM.mySingleArray[6];

		inv[10] = aM.mySingleArray[0] * aM.mySingleArray[5] * aM.mySingleArray[15] -
			aM.mySingleArray[0] * aM.mySingleArray[7] * aM.mySingleArray[13] -
			aM.mySingleArray[4] * aM.mySingleArray[1] * aM.mySingleArray[15] +
			aM.mySingleArray[4] * aM.mySingleArray[3] * aM.mySingleArray[13] +
			aM.mySingleArray[12] * aM.mySingleArray[1] * aM.mySingleArray[7] -
			aM.mySingleArray[12] * aM.mySingleArray[3] * aM.mySingleArray[5];

		inv[14] = -aM.mySingleArray[0] * aM.mySingleArray[5] * aM.mySingleArray[14] +
			aM.mySingleArray[0] * aM.mySingleArray[6] * aM.mySingleArray[13] +
			aM.mySingleArray[4] * aM.mySingleArray[1] * aM.mySingleArray[14] -
			aM.mySingleArray[4] * aM.mySingleArray[2] * aM.mySingleArray[13] -
			aM.mySingleArray[12] * aM.mySingleArray[1] * aM.mySingleArray[6] +
			aM.mySingleArray[12] * aM.mySingleArray[2] * aM.mySingleArray[5];

		inv[3] = -aM.mySingleArray[1] * aM.mySingleArray[6] * aM.mySingleArray[11] +
			aM.mySingleArray[1] * aM.mySingleArray[7] * aM.mySingleArray[10] +
			aM.mySingleArray[5] * aM.mySingleArray[2] * aM.mySingleArray[11] -
			aM.mySingleArray[5] * aM.mySingleArray[3] * aM.mySingleArray[10] -
			aM.mySingleArray[9] * aM.mySingleArray[2] * aM.mySingleArray[7] +
			aM.mySingleArray[9] * aM.mySingleArray[3] * aM.mySingleArray[6];

		inv[7] = aM.mySingleArray[0] * aM.mySingleArray[6] * aM.mySingleArray[11] -
			aM.mySingleArray[0] * aM.mySingleArray[7] * aM.mySingleArray[10] -
			aM.mySingleArray[4] * aM.mySingleArray[2] * aM.mySingleArray[11] +
			aM.mySingleArray[4] * aM.mySingleArray[3] * aM.mySingleArray[10] +
			aM.mySingleArray[8] * aM.mySingleArray[2] * aM.mySingleArray[7] -
			aM.mySingleArray[8] * aM.mySingleArray[3] * aM.mySingleArray[6];

		inv[11] = -aM.mySingleArray[0] * aM.mySingleArray[5] * aM.mySingleArray[11] +
			aM.mySingleArray[0] * aM.mySingleArray[7] * aM.mySingleArray[9] +
			aM.mySingleArray[4] * aM.mySingleArray[1] * aM.mySingleArray[11] -
			aM.mySingleArray[4] * aM.mySingleArray[3] * aM.mySingleArray[9] -
			aM.mySingleArray[8] * aM.mySingleArray[1] * aM.mySingleArray[7] +
			aM.mySingleArray[8] * aM.mySingleArray[3] * aM.mySingleArray[5];

		inv[15] = aM.mySingleArray[0] * aM.mySingleArray[5] * aM.mySingleArray[10] -
			aM.mySingleArray[0] * aM.mySingleArray[6] * aM.mySingleArray[9] -
			aM.mySingleArray[4] * aM.mySingleArray[1] * aM.mySingleArray[10] +
			aM.mySingleArray[4] * aM.mySingleArray[2] * aM.mySingleArray[9] +
			aM.mySingleArray[8] * aM.mySingleArray[1] * aM.mySingleArray[6] -
			aM.mySingleArray[8] * aM.mySingleArray[2] * aM.mySingleArray[5];

		T det = aM.mySingleArray[0] * inv[0] + aM.mySingleArray[1] * inv[4] + aM.mySingleArray[2] * inv[8] + aM.mySingleArray[3] * inv[12];


		det = T(1.0) / det;
		Matrix4x4<T> returnMatrix;
		for (int i = 0; i < 16; i++)
		{
			returnMatrix.mySingleArray[i] = inv[i] * det;
		}

		return returnMatrix;
	}

	template<class T>
	Vec4<T> Matrix4x4<T>::GetRight() const
	{
		return myRows[0];
	}
	template<class T>
	Vec4<T>& Matrix4x4<T>::GetRight() 
	{
		return myRows[0];
	}

	template<class T>
	Vec3<T> Matrix4x4<T>::GetRightV3() const
	{
		return myRight;
	}
	template<class T>
	Vec3<T>& Matrix4x4<T>::GetRightV3()
	{
		return myRight;
	}


	template<class T>
	Vec4<T> Matrix4x4<T>::GetUp() const
	{
		return myRows[1];
	}
	template<class T>
	Vec4<T>& Matrix4x4<T>::GetUp()
	{
		return myRows[1];
	}

	template<class T>
	Vec3<T> Matrix4x4<T>::GetUpV3() const
	{
		return myUp;
	}
	template<class T>
	Vec3<T>& Matrix4x4<T>::GetUpV3()
	{
		return myUp;
	}

	template<class T>
	Vec4<T> Matrix4x4<T>::GetForward() const
	{
		return myRows[2];
	}
	template<class T>
	Vec4<T>& Matrix4x4<T>::GetForward()
	{
		return myRows[2];
	}
	
	template<class T>
	Vec3<T> Matrix4x4<T>::GetForwardV3() const
	{
		return myForward;
	}
	template<class T>
	Vec3<T>& Matrix4x4<T>::GetForwardV3()
	{
		return myForward;
	}

	template<class T>
	Vec4<T> Matrix4x4<T>::GetTranslation() const
	{
		return myRows[3];
	}
	template<class T>
	Vec4<T>& Matrix4x4<T>::GetTranslation()
	{
		return myRows[3];
	}

	template<class T>
	Vec3<T> Matrix4x4<T>::GetTranslationV3() const
	{
		return myTranslation;
	}
	template<class T>
	Vec3<T>& Matrix4x4<T>::GetTranslationV3()
	{
		return myTranslation;
	}

	template<class T>
	Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aRightMatrix) const
	{
		Matrix4x4<T> tempMatrix = *this;

		tempMatrix.myDoubleArray[0][0] += aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[0][1] += aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[0][2] += aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[0][3] += aRightMatrix.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[1][0] += aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[1][1] += aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[1][2] += aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[1][3] += aRightMatrix.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[2][0] += aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[2][1] += aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[2][2] += aRightMatrix.myDoubleArray[2][2];
		tempMatrix.myDoubleArray[2][3] += aRightMatrix.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[3][0] += aRightMatrix.myDoubleArray[3][0];
		tempMatrix.myDoubleArray[3][1] += aRightMatrix.myDoubleArray[3][1];
		tempMatrix.myDoubleArray[3][2] += aRightMatrix.myDoubleArray[3][2];
		tempMatrix.myDoubleArray[3][3] += aRightMatrix.myDoubleArray[3][3];

		return tempMatrix;
	} 
	template<class T>
	Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aRightMatrix) const
	{
		Matrix4x4<T> tempMatrix = *this;

		tempMatrix.myDoubleArray[0][0] -= aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[0][1] -= aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[0][2] -= aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[0][3] -= aRightMatrix.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[1][0] -= aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[1][1] -= aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[1][2] -= aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[1][3] -= aRightMatrix.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[2][0] -= aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[2][1] -= aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[2][2] -= aRightMatrix.myDoubleArray[2][2];
		tempMatrix.myDoubleArray[2][3] -= aRightMatrix.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[3][0] -= aRightMatrix.myDoubleArray[3][0];
		tempMatrix.myDoubleArray[3][1] -= aRightMatrix.myDoubleArray[3][1];
		tempMatrix.myDoubleArray[3][2] -= aRightMatrix.myDoubleArray[3][2];
		tempMatrix.myDoubleArray[3][3] -= aRightMatrix.myDoubleArray[3][3];

		return tempMatrix;
	} 
	template<class T>
	Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aRightMatrix) const
	{
		Matrix4x4<T> tempMatrix;
		tempMatrix.myDoubleArray[0][0] = 0;
		tempMatrix.myDoubleArray[1][1] = 0;
		tempMatrix.myDoubleArray[2][2] = 0;
		tempMatrix.myDoubleArray[3][3] = 0;

		//ROW ONE

		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[0][0] += this->myDoubleArray[0][3] * aRightMatrix.myDoubleArray[3][0];

		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[0][1] += this->myDoubleArray[0][3] * aRightMatrix.myDoubleArray[3][1];

		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][2];
		tempMatrix.myDoubleArray[0][2] += this->myDoubleArray[0][3] * aRightMatrix.myDoubleArray[3][2];

		tempMatrix.myDoubleArray[0][3] += this->myDoubleArray[0][0] * aRightMatrix.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[0][3] += this->myDoubleArray[0][1] * aRightMatrix.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[0][3] += this->myDoubleArray[0][2] * aRightMatrix.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[0][3] += this->myDoubleArray[0][3] * aRightMatrix.myDoubleArray[3][3];

		//ROW TWO

		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[1][0] += this->myDoubleArray[1][3] * aRightMatrix.myDoubleArray[3][0];

		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[1][1] += this->myDoubleArray[1][3] * aRightMatrix.myDoubleArray[3][1];

		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][2];
		tempMatrix.myDoubleArray[1][2] += this->myDoubleArray[1][3] * aRightMatrix.myDoubleArray[3][2];

		tempMatrix.myDoubleArray[1][3] += this->myDoubleArray[1][0] * aRightMatrix.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[1][3] += this->myDoubleArray[1][1] * aRightMatrix.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[1][3] += this->myDoubleArray[1][2] * aRightMatrix.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[1][3] += this->myDoubleArray[1][3] * aRightMatrix.myDoubleArray[3][3];

		//ROW THREE

		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[2][0] += this->myDoubleArray[2][3] * aRightMatrix.myDoubleArray[3][0];

		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[2][1] += this->myDoubleArray[2][3] * aRightMatrix.myDoubleArray[3][1];

		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][2];
		tempMatrix.myDoubleArray[2][2] += this->myDoubleArray[2][3] * aRightMatrix.myDoubleArray[3][2];

		tempMatrix.myDoubleArray[2][3] += this->myDoubleArray[2][0] * aRightMatrix.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[2][3] += this->myDoubleArray[2][1] * aRightMatrix.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[2][3] += this->myDoubleArray[2][2] * aRightMatrix.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[2][3] += this->myDoubleArray[2][3] * aRightMatrix.myDoubleArray[3][3];

		//ROW FOUR

		tempMatrix.myDoubleArray[3][0] += this->myDoubleArray[3][0] * aRightMatrix.myDoubleArray[0][0];
		tempMatrix.myDoubleArray[3][0] += this->myDoubleArray[3][1] * aRightMatrix.myDoubleArray[1][0];
		tempMatrix.myDoubleArray[3][0] += this->myDoubleArray[3][2] * aRightMatrix.myDoubleArray[2][0];
		tempMatrix.myDoubleArray[3][0] += this->myDoubleArray[3][3] * aRightMatrix.myDoubleArray[3][0];

		tempMatrix.myDoubleArray[3][1] += this->myDoubleArray[3][0] * aRightMatrix.myDoubleArray[0][1];
		tempMatrix.myDoubleArray[3][1] += this->myDoubleArray[3][1] * aRightMatrix.myDoubleArray[1][1];
		tempMatrix.myDoubleArray[3][1] += this->myDoubleArray[3][2] * aRightMatrix.myDoubleArray[2][1];
		tempMatrix.myDoubleArray[3][1] += this->myDoubleArray[3][3] * aRightMatrix.myDoubleArray[3][1];

		tempMatrix.myDoubleArray[3][2] += this->myDoubleArray[3][0] * aRightMatrix.myDoubleArray[0][2];
		tempMatrix.myDoubleArray[3][2] += this->myDoubleArray[3][1] * aRightMatrix.myDoubleArray[1][2];
		tempMatrix.myDoubleArray[3][2] += this->myDoubleArray[3][2] * aRightMatrix.myDoubleArray[2][2];
		tempMatrix.myDoubleArray[3][2] += this->myDoubleArray[3][3] * aRightMatrix.myDoubleArray[3][2];

		tempMatrix.myDoubleArray[3][3] += this->myDoubleArray[3][0] * aRightMatrix.myDoubleArray[0][3];
		tempMatrix.myDoubleArray[3][3] += this->myDoubleArray[3][1] * aRightMatrix.myDoubleArray[1][3];
		tempMatrix.myDoubleArray[3][3] += this->myDoubleArray[3][2] * aRightMatrix.myDoubleArray[2][3];
		tempMatrix.myDoubleArray[3][3] += this->myDoubleArray[3][3] * aRightMatrix.myDoubleArray[3][3];

		return tempMatrix;
	} 
	template<class T>
	Matrix4x4<T> Matrix4x4<T>::operator*(const T& aRightScalar) const
	{
		Matrix4x4<T> tempMatrix = *this;

		tempMatrix.myDoubleArray[0][0] *= aRightScalar;
		tempMatrix.myDoubleArray[0][1] *= aRightScalar;
		tempMatrix.myDoubleArray[0][2] *= aRightScalar;
		tempMatrix.myDoubleArray[0][3] *= aRightScalar;
		tempMatrix.myDoubleArray[1][0] *= aRightScalar;
		tempMatrix.myDoubleArray[1][1] *= aRightScalar;
		tempMatrix.myDoubleArray[1][2] *= aRightScalar;
		tempMatrix.myDoubleArray[1][3] *= aRightScalar;
		tempMatrix.myDoubleArray[2][0] *= aRightScalar;
		tempMatrix.myDoubleArray[2][1] *= aRightScalar;
		tempMatrix.myDoubleArray[2][2] *= aRightScalar;
		tempMatrix.myDoubleArray[2][3] *= aRightScalar;
		tempMatrix.myDoubleArray[3][0] *= aRightScalar;
		tempMatrix.myDoubleArray[3][1] *= aRightScalar;
		tempMatrix.myDoubleArray[3][2] *= aRightScalar;
		tempMatrix.myDoubleArray[3][3] *= aRightScalar;

		return tempMatrix;
	}
				
	template<class T>
	Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4<T>& aRightMatrix)
	{
		*this = *this + aRightMatrix;
		return *this;

	}
	template<class T>
	Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& aRightMatrix)
	{
		*this = *this - aRightMatrix;
		return *this;
	}
	template<class T>
	Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aRightMatrix)
	{
		*this = *this * aRightMatrix;

		return *this;
	}
	template<class T>
	Matrix4x4<T>& Matrix4x4<T>::operator*=(const T& aRightScalar)
	{
		*this = *this * aRightScalar;

		return *this;
	}


	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrix(Vec3<T> aVec) {
		return CreateRotationMatrix(aVec.x, aVec.y, aVec.z);
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrix(T aX, T aY, T aZ) {
		Matrix4x4<T> m;
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
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> tempRotationMatrix;
		tempRotationMatrix.myDoubleArray[0][0] = (T)1;
		tempRotationMatrix.myDoubleArray[1][1] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][2] = (T)sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][1] = (T)-sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][2] = (T)cos(aAngleInRadians);

		return tempRotationMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> tempRotationMatrix;
		tempRotationMatrix.myDoubleArray[0][0] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[0][2] = (T)-sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][1] = (T)1;
		tempRotationMatrix.myDoubleArray[2][0] = (T)sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][2] = (T)cos(aAngleInRadians);

		return tempRotationMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> tempRotationMatrix;
		tempRotationMatrix.myDoubleArray[0][0] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[0][1] = (T)sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][0] = (T)-sin(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[1][1] = (T)cos(aAngleInRadians);
		tempRotationMatrix.myDoubleArray[2][2] = (T)1;

		return tempRotationMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundAxis(Vec4<T> aVec, T aAngleInRadians)
	{
		Matrix4x4<T> I;
		//cross matrix
		Matrix4x4<T> K;
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
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(Vec3<T> aScale)
	{
		return CreateScaleMatrix(aScale.x, aScale.y, aScale.z);
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(T aX, T aY, T aZ) {
		Matrix4x4<T> m;
		m.myDoubleArray[0][0] = aX;
		m.myDoubleArray[1][1] = aY;
		m.myDoubleArray[2][2] = aZ;

		return m;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(Vec3<T> aPos)
	{
		return CreateTranslationMatrix(aPos.x, aPos.y, aPos.z);
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(T aX, T aY, T aZ){
		Matrix4x4<T> m;
		m.myDoubleArray[3][0] = aX;
		m.myDoubleArray[3][1] = aY;
		m.myDoubleArray[3][2] = aZ;
		return m;
	}

	template<class T>
	void Matrix4x4<T>::DecomposeMatrix(Vec3<T>& outRotation, Vec3<T>& outScale, Vec3<T>& outTranlation)
	{ 
		outTranlation	= myTranslation;
		outScale		= DecomposeScale();
		outRotation		= DecomposeRotation();
	}

	template<class T>
	CU::Vec3<T> Matrix4x4<T>::DecomposeRotation()
	{
		Vec3<T> xNormd = myRight.GetNormalized();
		Vec3<T> yNormd = myUp.GetNormalized();
		Vec3<T> zNormd = myForward.GetNormalized();

		Vec3<T> rot;
		rot.x = ISTE::RadToDeg * atan2f(yNormd.z, zNormd.z);
		rot.y = ISTE::RadToDeg * atan2f(-xNormd.z, sqrtf(yNormd.z * yNormd.z + zNormd.z * zNormd.z));
		rot.z = ISTE::RadToDeg * atan2f(xNormd.y, xNormd.x);

		return rot;
	}
	template<class T>
	CU::Vec3<T> Matrix4x4<T>::DecomposeScale()
	{ 
		return{
			myRight.Length(),
			myUp.Length(),
			myForward.Length()
		};
	}
	

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateProjectionMatrix(const float aHorizontalFovInDeg, const CU::Vec2Ui& aRes, const float aNearPlane, const float aFarPlane) {
		Matrix4x4<T> projectionMatrix;

		projectionMatrix(1, 1) = 1.f / (float)(tan(aHorizontalFovInDeg / 2.f));
		projectionMatrix(2, 2) = ((float)aRes.x / (float)aRes.y) * (float)(1.f / (tan(aHorizontalFovInDeg / 2.f)));
		projectionMatrix(3, 3) = aFarPlane / (aFarPlane - aNearPlane);
		projectionMatrix(3, 4) = 1.f;
		projectionMatrix(4, 3) = (-aNearPlane * aFarPlane) / (aFarPlane - aNearPlane);
		projectionMatrix(4, 4) = 0;

		return projectionMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateOrthographicMatrix(const CU::Vec2Ui& aRes, const float aNearPlane, const float aFarPlane) {
		Matrix4x4<T> orthographicMatrix;

		orthographicMatrix(1, 1) = 2.f / (float)aRes.x;
		orthographicMatrix(2, 2) = 2.f / (float)aRes.y;
		orthographicMatrix(3, 3) = 1.f / (float)(aFarPlane - aNearPlane);
		orthographicMatrix(4, 3) = (aNearPlane) / (float)(aNearPlane - aFarPlane);
		orthographicMatrix(4, 4) = 1;

		return orthographicMatrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateOrthographicMatrix(const T aLeft, const T aRight, const T aBottom, const T aTop, const T aNear, const T aFar)
	{
		Matrix4x4<T> result;
		result.mySingleArray[0] = T(2) / (aRight - aLeft);
		result.mySingleArray[1] = T(0);
		result.mySingleArray[2] = T(0);
		result.mySingleArray[3] = T(0);

		//Second row
		result.mySingleArray[4] = T(0);
		result.mySingleArray[5] = T(2) / (aTop - aBottom);
		result.mySingleArray[6] = T(0);
		result.mySingleArray[7] = T(0);

		//Third row
		result.mySingleArray[8] = T(0);
		result.mySingleArray[9] = T(0);
		result.mySingleArray[10] = T(1) / (aFar - aNear);
		result.mySingleArray[11] = T(0);

		//Fourth row
		result.mySingleArray[12] = (aLeft + aRight) / (aLeft - aRight);
		result.mySingleArray[13] = (aTop + aBottom) / (aBottom - aTop);
		result.mySingleArray[14] = aNear / (aNear - aFar);
		result.mySingleArray[15] = T(1);
		return result;
	}
	
	template<class T>
	inline void Matrix4x4<T>::SetScale(const Vec3<T>& aVec3)
	{
		myRight.Normalize();
		myUp.Normalize();
		myForward.Normalize(); 

		myRight		*= aVec3.x;
		myUp		*= aVec3.y;
		myForward	*= aVec3.z;
	} 

	//rethink this		should i just put this in the vector class instead. 
	template<class T>
	Vec4<T> operator*(const Vec4<T>& aLeftVector, const Matrix4x4<T>& aRightMatrix)
	{ 
		Matrix4x4<T> tempMatrix = aRightMatrix;

		tempMatrix(1, 1) *= aLeftVector.x;
		tempMatrix(1, 2) *= aLeftVector.x;
		tempMatrix(1, 3) *= aLeftVector.x;
		tempMatrix(1, 4) *= aLeftVector.x;

		tempMatrix(2, 1) *= aLeftVector.y;
		tempMatrix(2, 2) *= aLeftVector.y;
		tempMatrix(2, 3) *= aLeftVector.y;
		tempMatrix(2, 4) *= aLeftVector.y;

		tempMatrix(3, 1) *= aLeftVector.z;
		tempMatrix(3, 2) *= aLeftVector.z;
		tempMatrix(3, 3) *= aLeftVector.z;
		tempMatrix(3, 4) *= aLeftVector.z;

		tempMatrix(4, 1) *= aLeftVector.w;
		tempMatrix(4, 2) *= aLeftVector.w;
		tempMatrix(4, 3) *= aLeftVector.w;
		tempMatrix(4, 4) *= aLeftVector.w;

		return Vec4<T>(
			tempMatrix(1, 1) + tempMatrix(2, 1) + tempMatrix(3, 1) + tempMatrix(4, 1),
			tempMatrix(1, 2) + tempMatrix(2, 2) + tempMatrix(3, 2) + tempMatrix(4, 2),
			tempMatrix(1, 3) + tempMatrix(2, 3) + tempMatrix(3, 3) + tempMatrix(4, 3),
			tempMatrix(1, 4) + tempMatrix(2, 4) + tempMatrix(3, 4) + tempMatrix(4, 4)
			);
	}
	template<class T>
	Matrix4x4<T> operator*(const T aLeftScalar, const Matrix4x4<T>& aRightMatrix)
	{
		return aRightMatrix * aLeftScalar;
	}

}
#pragma warning(pop)