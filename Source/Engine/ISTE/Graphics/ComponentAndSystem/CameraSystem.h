#pragma once

#include <ISTE/ECSB/System.h>
#include <ISTE/Math/Vec2.h>

namespace ISTE
{
	struct Context;
	class Camera;

	class CameraSystem : System
	{
	public:
		~CameraSystem() = default;

		const bool Init();
		void Update();

		void UpdateCameraResolution(const CU::Vec2Ui& aResolution);

		//void LookAt(const CU::Vec3f aPosition);

	private: // editor camera
		void EditorCameraControl(const float aTimeDelta);
		void Move(const float aTimeDelta);
		void LookAround(const float aTimeDelta);
		void OrhoNormalization();
		float myMovementSpeed;
		float myMovementSpeedMax;
		float myLookAroundSpeed;
		float myHeadPitch;
		float myHeadYaw;
		float myMaxPitch;
		float myMinPitch;
	
	private:
		Context* myCtx = nullptr;
		Camera* myCurrentCamera = nullptr;
	};
}