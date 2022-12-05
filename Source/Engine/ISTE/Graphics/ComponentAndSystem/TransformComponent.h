#pragma once 
#include <ISTE/Math/Vec3.h>
#include <ISTE/Math/Matrix4x4.h>
#include <ISTE/Math/Quaternion.h>
#include <ISTE/Math/Euler.h>

//all implicitly created special functions got deleted?
//maybe because of the union or something within the union
//have to check later
//spcial functions explicitly created in the mean time
#pragma warning(push)
#pragma warning(disable: 4201)
namespace ISTE
{
	struct TransformComponent
	{
		TransformComponent(TransformComponent& aLeft) { 
			memcpy(this, &aLeft, sizeof(TransformComponent));
		};
		TransformComponent() {};
		~TransformComponent() {};
		TransformComponent& operator= (TransformComponent& other) 
		{ 
			memcpy(this, &other, sizeof(TransformComponent)); 
			return *this;
		}

		CU::Vec3f myPosition;
		union { 
			CU::Euler myEuler = {};
			CU::Quaternionf myQuaternion;
		};
		CU::Vec3f myScale = {1,1,1};
		
				
		CU::Matrix4x4f myCachedTransform;
	};
}
#pragma warning(pop)