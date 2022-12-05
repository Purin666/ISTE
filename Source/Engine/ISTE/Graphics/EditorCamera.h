#pragma once
#include <ISTE/Math/Matrix4x4.h>
#include <ISTE/Math/Vec.h>

namespace CU {
	class InputHandler;
}

namespace ISTE {
	class EditorCamera
	{
	public:
		EditorCamera();
		~EditorCamera();

		void Init(); 
		void Update(float);

		void SetFov(float aDegrees);

		void SetPerspectiveProjection(float aHorizontalFovInDeg, CU::Vec2Ui aRes, float aNearPlane, float aFarPlane);

		CU::Vec3f GetPosition();
		CU::Matrix4x4f& GetTransform() { return myTransform; }

		const CU::Matrix4x4f& GetProjectionMatrix() { return myProjectionMatrix; }
		CU::Matrix4x4f GetWorldToClip();

	private:
		float myFov;
		float myNearPlane;
		float myFarPlane;

		CU::Vec2Ui myResolution;

		CU::Matrix4x4f myProjectionMatrix;
		CU::Matrix4x4f myTransform;

	private:
		//movement stuff
		void Move(float);
		void LookAround(float);

		void OrhoNormalization();

		float myMovementSpeed;
		float myLookAroundSpeed;

		float myHeadPitch;
		float myHeadYaw;
		float myMaxPitch;
		float myMinPitch;

		CU::InputHandler* myIH;
	};
}