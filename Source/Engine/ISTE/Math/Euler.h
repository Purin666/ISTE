#pragma once
#include "Quaternion.h"
#include "Vec3.h"
#include "Matrix4x4.h"

/*
*  Euler.h			Created and last edited by George 07-26
* ---------------------------------------------------------
* 
* Literally just a middle man to make rotation easier to handle. 
* Saves the angles localy 
*	-	quaternions are capped between -180 to 180 and is handled entirely differently 
*		so saving it localy to make it more human friendly
* 
* Just like any singular representation of rotation; axis angle and euler, 
* This will still fall ill to gimbal lock :/
*	-	Altough, theres probably a way to fix that.
*
* im probably forgetting to add something right now
*/

namespace CU
{
	class Euler
	{
	private:
		Quaternionf myQuaternion = { 0,0,0,1 };
		Vec3f myAngles = {0,0,0};
	public: 
		//assignment operator 
		Euler& operator=(const Euler& aVector3)
		{
			memcpy(this, &aVector3, sizeof(Euler));
			return *this;
		}
		Euler() {};
		//In Degrees
		void SetRotation(Vec3f aXYZRotation);
		//In Degrees
		void SetRotation(float aX, float aY, float aZ);

		//In Degrees
		void AddRotation(Vec3f aXYZRotation);
		//In Degrees
		void AddRotation(float aX, float aY, float aZ);
		Vec3f GetAngles();

		Matrix4x4f GetRotationMatrix();
		void AnglesUpdated();
		void ToEulerAngles();
	};
};
