#include "CameraSystem.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include <ISTE/Scene/Scene.h>
#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Time/TimeHandler.h>
#include "ISTE/CU/InputHandler.h"
#include <ISTE/Builder/BuilderDataStructs.h> // struct Camera

#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/Camera.h>

#include <ISTE/CU/HashMap.hpp>


//test //lukas
#include "ISTE/Graphics/ComponentAndSystem/CullingSystem.h"

namespace ISTE
{
	const bool CameraSystem::Init()
	{
		myCtx = Context::Get();
		myCurrentCamera = &myCtx->mySceneHandler->GetCamera();
		
		myMovementSpeed = 1.f;
		myMovementSpeedMax = 10.f;
		myLookAroundSpeed = 1.f;
		myMaxPitch = 89.f * ISTE::DegToRad;
		myMinPitch = -145.f * ISTE::DegToRad;
		myHeadPitch = 0.f;
		myHeadYaw = 0.f;

		return true;
	}

	void CameraSystem::Update()
	{
		const float timeDelta = myCtx->myTimeHandler->GetDeltaTime();

		//test //lukas
		if(Context::Get()->mySceneHandler->IsSceneActive())
			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->Cull();

		if (true /*enable editor camera*/) EditorCameraControl(timeDelta);
	}

	void CameraSystem::UpdateCameraResolution(const CU::Vec2Ui& aResolution)
	{
		const float fov = myCurrentCamera->myFov;
		const float nearPlane = myCurrentCamera->myNearPlane;
		const float farPlane = myCurrentCamera->myFarPlane;
		myCurrentCamera->SetPerspectiveProjection(fov, aResolution, nearPlane, farPlane);
	}

	void CameraSystem::EditorCameraControl(const float aTimeDelta)
	{
		auto input = myCtx->myInputHandler;
		if (input->IsKeyUp(MK_RBUTTON))
			myMovementSpeed = 1.f;
		if (!input->IsKeyHeldDown(MK_RBUTTON))
			return;

		myMovementSpeed += aTimeDelta * 3.f;

		Move(aTimeDelta);
		LookAround(aTimeDelta);
	}

	void CameraSystem::Move(const float aTimeDelta)
	{
		CU::Vec3f right = { cosf(-myHeadYaw), 0.f ,sinf(-myHeadYaw) };
		CU::Vec3f forward = { sinf(myHeadYaw), 0.f ,cosf(myHeadYaw) };

		auto& inputMgr = myCtx->myInputHandler;

		float mult = 1;

		if (inputMgr->IsKeyHeldDown(VK_SHIFT))
			mult = 4;

		right	*= myMovementSpeed * aTimeDelta * mult;
		forward *= myMovementSpeed * aTimeDelta * mult;

		CU::Matrix4x4f& transform = myCurrentCamera->myTransform;
		
		//movement in z
		if (inputMgr->IsKeyHeldDown('W'))
		{
			transform.GetRow(4).x += forward.x;
			transform.GetRow(4).z += forward.z;
		}
		if (inputMgr->IsKeyHeldDown('S'))
		{
			transform.GetRow(4).x += -forward.x;
			transform.GetRow(4).z += -forward.z;
		}

		//movement in x
		if (inputMgr->IsKeyHeldDown('D'))
		{
			transform.GetRow(4).x += right.x;
			transform.GetRow(4).z += right.z;
		}
		if (inputMgr->IsKeyHeldDown('A'))
		{
			transform.GetRow(4).x += -right.x;
			transform.GetRow(4).z += -right.z;
		}

		//movement in y
		if (inputMgr->IsKeyHeldDown('E'))
			transform(4, 2) += myMovementSpeed * aTimeDelta * mult;
		if (inputMgr->IsKeyHeldDown('Q'))
			transform(4, 2) -= myMovementSpeed * aTimeDelta * mult;
	}

	void CameraSystem::LookAround(const float /*aTimeDelta*/)
	{
		POINT mVel = myCtx->myInputHandler->GetMouseVelocity();

		CU::Matrix4x4f& transform = myCurrentCamera->myTransform;
		//transform directions
		auto& right = transform.GetRow(1);
		auto& up = transform.GetRow(2);
		auto& forward = transform.GetRow(3);

		const float fovFactor = myCurrentCamera->GetFov() * 0.0075f;
		float pitchAmount = mVel.y * myLookAroundSpeed * 0.01f * fovFactor;
		float yawAmount =   mVel.x * myLookAroundSpeed * 0.01f * fovFactor;


		myHeadYaw += yawAmount;
		myHeadPitch += pitchAmount;
		if (myHeadPitch < myMinPitch || myHeadPitch > myMaxPitch)	//clamps the pitch of the camera
		{
			myHeadPitch -= pitchAmount;
			pitchAmount = 0;
		}

		CU::Matrix4x4f pitchAroundX = CU::Matrix4x4f::CreateRotationAroundAxis(right, pitchAmount);
		CU::Matrix4x4f yaw = CU::Matrix4x4f::CreateRotationAroundY(yawAmount);

		//pitch around transforms right
		forward = forward * pitchAroundX;
		up = up * pitchAroundX;

		//yaw around worlds up 
		up = up * yaw;
		forward = forward * yaw;
		right = right * yaw;

		OrhoNormalization();
	}

	void CameraSystem::OrhoNormalization()
	{
		//this is honestly really ugly 
		//should make it so that you can turn vec4s in to vec3 just by the operator=
		//george
		
		CU::Matrix4x4f& transform = myCurrentCamera->myTransform;

		CU::Vec3f right = { transform(1,1), transform(1,2),transform(1,3) };
		CU::Vec3f up = { transform(2,1), transform(2,2),transform(2,3) };
		CU::Vec3f forward = { transform(3,1), transform(3,2),transform(3,3) };

		forward.Normalize();
		up = forward.Cross(right).GetNormalized();
		right = up.Cross(forward).GetNormalized();

		transform(1, 1) = right.x;
		transform(1, 2) = right.y;
		transform(1, 3) = right.z;

		transform(2, 1) = up.x;
		transform(2, 2) = up.y;
		transform(2, 3) = up.z;

		transform(3, 1) = forward.x;
		transform(3, 2) = forward.y;
		transform(3, 3) = forward.z;
	}

	//void CameraSystem::LookAt(const CU::Vec3f aPosition)
	//{
	//	//const CU::Vec3f diff = myPosition - (aPosition);
	//	//myRotation.y = std::atan2f(diff.z, -diff.x);
	//	//
	//	//const float l = sqrtf(diff.z * diff.z + diff.x * diff.x);
	//	//myRotation.x = std::atan2f(-l, diff.y);
	//	//
	//	//myRotation.x += PI * 0.5f;
	//	//myRotation.y += PI * 0.5f;
	//}
}
