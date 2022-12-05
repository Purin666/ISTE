#pragma once
#include "ISTE/Math/Vec2.h"

namespace CU {
	template<class T>
	class Matrix2x2{
	public:
		union {
			T myData[4] = {
				1,0
				0,1
			};
			struct { T m11, m12, m21, m22 };
			struct {CU::Vec2<T> myRight, myUp};
		};

		Matrix2x2<T>();
		Matrix2x2<T>(const Matrix2x2<T>& aMatrix);
		Matrix2x2<T>(std::initializer_list<T> aList);

		Matrix2x2<T>& operator=(const Matrix2x2<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		bool operator==(const Matrix2x2<T>& aMatrix) const;
		bool operator!=(const Matrix2x2<T>& aMatrix) const;

		Matrix2x2<T> operator+(const Matrix2x2<T>& aMatrix) const;
		Matrix2x2<T> operator-(const Matrix2x2<T>& aMatrix) const;
		Matrix2x2<T> operator*(const Matrix2x2<T>& aMatrix) const;
		Matrix2x2<T> operator*(const T& aScalar) const;
		Vec2<T> operator*(const Vec2<T>& aVec) const;
		Matrix2x2<T>& operator+=(const Matrix2x2<T>& aMatrix);
		Matrix2x2<T>& operator-=(const Matrix2x2<T>& aMatrix);
		Matrix2x2<T>& operator*=(const Matrix2x2<T>& aMatrix);
		Matrix2x2<T>& operator*=(const T& aScalar);

		static Matrix2x2<T> CreateRotation(const T aAngleInRadians);
		static Matrix2x2<T> CreateScaleMatrix(Vec2<T> aScalarVec);
		static Matrix2x2<T> Transpose(const Matrix2x2<T>& aMatrixToTranspose);
	};
	typedef Matrix2x2<float> Matrix2x2f;

#pragma region Constructors
	template <typename T> Matrix2x2<T>::Matrix2x2() : myData()
	{
		std::memset(myData, 0, myLength * sizeof(T));
		myData[0] = 1;
		myData[3] = 1;

	}
	template <typename T> Matrix2x2<T>::Matrix2x2(const Matrix2x2<T>& aMatrix)
	{
		memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
	}

	template<typename T> inline Matrix2x2<T>::Matrix2x2(std::initializer_list<T> aList)
	{
		assert(aList.size() <= myLength && "Initializer list contains too many values.");
		std::memcpy(myData, aList.begin(), sizeof(T) * aList.size());
		if (aList.size() < myLength)
		{
			std::fill(myData + aList.size(), myData + myLength, myData[aList.size() - 1]);
		}
	}

#pragma endregion Constructors

#pragma region Operators

	template<typename T>
	inline Matrix2x2<T>& Matrix2x2<T>::operator=(const Matrix2x2<T>& aMatrix)
	{
		std::memcpy(myData, aMatrix.myData, sizeof(T) * myLength);
		return *this;
	}

	// Rows and Columns start at 1.
	template<typename T> inline T& Matrix2x2<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow < 3 && aColumn > 0 && aColumn < 3 && "Argument out of bounds");
		return myData[(aRow - 1) * 2 + (aColumn - 1)];
	}

	// Rows and Columns start at 1.
	template<typename T> inline const T& Matrix2x2<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow < 3 && aColumn > 0 && aColumn < 3 && "Argument out of bounds");
		return myData[(aRow - 1) * 2 + (aColumn - 1)];
	}

	template<typename T>
	inline bool Matrix2x2<T>::operator==(const Matrix2x2<T>& aMatrix) const
	{
		for (int i = 0; i < myLength; i++)
		{
			if (myData[i] != aMatrix.myData[i])
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline bool Matrix2x2<T>::operator!=(const Matrix2x2<T>& aMatrix) const
	{
		return !operator==(aMatrix);
	}

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::operator+(const Matrix2x2<T>& aMatrix) const
	{
		Matrix2x2<T> result{ *this };
		return result += aMatrix;
	}

	template<typename T>
	inline Matrix2x2<T>& Matrix2x2<T>::operator+=(const Matrix2x2<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] += aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::operator-(const Matrix2x2<T>& aMatrix) const
	{
		Matrix2x2<T> result{ *this };
		return result -= aMatrix;
	}

	template<typename T>
	inline Matrix2x2<T>& Matrix2x2<T>::operator-=(const Matrix2x2<T>& aMatrix)
	{
		for (auto i = 0; i < myLength; i++)
		{
			myData[i] -= aMatrix.myData[i];
		}
		return *this;
	}

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::operator*(const Matrix2x2<T>& aMatrix) const
	{
		Matrix2x2<T> result;

		result.myData[0] = this->myData[0] * aMatrix.myData[0] + this->myData[1] * aMatrix.myData[2];
		result.myData[1] = this->myData[0] * aMatrix.myData[1] + this->myData[1] * aMatrix.myData[3];
		result.myData[2] = this->myData[2] * aMatrix.myData[0] + this->myData[3] * aMatrix.myData[2];
		result.myData[3] = this->myData[2] * aMatrix.myData[1] + this->myData[3] * aMatrix.myData[3];

		return result;
	}
	template<typename T>
	inline Matrix2x2<T>& Matrix2x2<T>::operator*=(const Matrix2x2<T>& aMatrix)
	{
		*this = *this * aMatrix;
		return *this;
	}

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::operator*(const T& aScalar) const
	{
		Matrix2x2<T> result{ *this };
		return result *= aScalar;
	}
	template<typename T>
	inline Vec2<T> Matrix2x2<T>::operator*(const Vec2<T>& aVec) const
	{
		Vec2<T> result;
		result.X = (myData[0] * aVec.X) + (myData[2] * aVec.Y);
		result.Y = (myData[1] * aVec.X) + (myData[3] * aVec.Y);
		return result;
	}
	template <typename T>
	Vec2<T> operator*(const Vec2<T>& aVec, const Matrix2x2<T> aMatrix)
	{
		Vec2<T> result;
		result.X = (aMatrix(1, 1) * aVec.X) + (aMatrix(2, 1) * aVec.Y);
		result.Y = (aMatrix(1, 2) * aVec.X) + (aMatrix(2, 2) * aVec.Y);
		return result;
	};

	template<typename T>
	inline Matrix2x2<T>& Matrix2x2<T>::operator*=(const T& aScalar)
	{
		for (int i = 0; i < myLength; i++)
		{
			myData[i] *= aScalar;
		}
		return *this;
	}

#pragma endregion Operators

#pragma region Static Functions

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::CreateRotation(const T aAngleInRadians)
	{
		const T cos = std::cos(aAngleInRadians);
		const T sin = std::sin(aAngleInRadians);
		return
		{
			cos,  sin,
			-sin, cos,
		};
	}

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::CreateScaleMatrix(Vec2<T> aScaleVec)
	{
		Matrix2x2<T> result;
		result.myData[0] = aScaleVec.X;
		result.myData[3] = aScaleVec.Y;
		return result;
	}

	template<typename T>
	inline Matrix2x2<T> Matrix2x2<T>::Transpose(const Matrix2x2<T>& aMatrixToTranspose)
	{
		Matrix2x2<T> result;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				result.myData[i + j * 2] = aMatrixToTranspose.myData[j + i * 2];
			}
		}
		return result;
	}

#pragma endregion Static Functions

};





