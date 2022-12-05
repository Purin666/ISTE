#pragma once
#include <ISTE/Math/Matrix4x4.h>
#include <ISTE/Math/Vec.h>

namespace ISTE
{
	struct Plane
	{
		CU::Vec3f myNoraml;
		CU::Vec3f myPoint;
		float d;

		void InitWith3Points(CU::Vec3f aPoint0, CU::Vec3f aPoint1, CU::Vec3f aPoint2)
		{
			myPoint = aPoint0;
			CU::Vec3f point = aPoint1 - aPoint0;
			CU::Vec3f point2 = aPoint2 - aPoint0;
			myNoraml = point.Cross(point2);
			myNoraml.Normalize();
			d = (myNoraml * -1.f).Dot(myPoint);
		}

		float Distance(const CU::Vec3f aPoint) { return myNoraml.Dot(aPoint) + d; }
	};

	struct Frustum
	{
		const int myPlanesAmount = 6;
		Plane myPlanes[6];
	};

	class Camera
	{
		friend class CameraSystem;
	public:
		Camera();
		~Camera();
	
		void SetPerspectiveProjection(const float aHorizontalFovInDeg, const CU::Vec2Ui& aRes, const float aNearPlane, const float aFarPlane); 
		inline void SetProjectionMatrix(CU::Matrix4x4f& aProjectionMatrix) { myProjectionMatrix = aProjectionMatrix; }
		
		void SetFov(const float aHorizontalFovInDeg);
	
		const CU::Vec3f GetPosition() const;
		const CU::Matrix4x4f& GetTransform() const { return myTransform; }
		CU::Matrix4x4f& GetTransformNonConst() { return myTransform; }
	
		const CU::Matrix4x4f& GetProjectionMatrix() const { return myProjectionMatrix; }
		const CU::Matrix4x4f GetWorldToClip() const;
		
		inline const float GetNearPlane() { return myNearPlane; }
		inline const float GetFarPlane() { return myFarPlane; }
		inline const float GetFov() { return myFov; }
		inline const CU::Vec2Ui GetResolution() { return myResolution; }

		Frustum GetFrustum();

	private:
		float myFov;
		float myNearPlane;
		float myFarPlane;
	
		CU::Vec2Ui myResolution;
	
		Frustum myFrustum;

		CU::Matrix4x4f myProjectionMatrix;
		CU::Matrix4x4f myTransform;
	};
}