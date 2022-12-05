#include "EditorCamera.h" 
#include <ISTE/Math/Math.h>
#include <ISTE/Context.h>
#include <ISTE/CU/InputHandler.h>

#include <iostream>

ISTE::EditorCamera::EditorCamera()
{
}

ISTE::EditorCamera::~EditorCamera()
{
}

void ISTE::EditorCamera::Init()
{
	myIH = Context::Get()->myInputHandler;
	myMovementSpeed = 1.f;
	myLookAroundSpeed = 0.01f;
	myMaxPitch = 89.f * ISTE::DegToRad;
	myMinPitch = -145.f * ISTE::DegToRad;
	myHeadPitch = 0.f;
	myHeadYaw = 0.f;
}

void ISTE::EditorCamera::Update(float aTimeDelta)
{
	if (myIH->IsKeyHeldDown('R') && myIH->IsKeyHeldDown(VK_CONTROL))
	{
		myHeadPitch = 0;
		myHeadYaw = 0;
		myTransform = {};
	}
	Move(aTimeDelta);
	LookAround(aTimeDelta);
}

void ISTE::EditorCamera::SetFov(float aDegrees)
{
	myFov = aDegrees * ISTE::DegToRad;
	SetPerspectiveProjection(myFov, myResolution, myNearPlane, myFarPlane);
}

void ISTE::EditorCamera::SetPerspectiveProjection(float aHorizontalFovInDeg, CU::Vec2Ui aRes, float aNearPlane, float aFarPlane)
{
	myFov = aHorizontalFovInDeg * ISTE::DegToRad;
	myResolution = aRes;
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;

	myProjectionMatrix(1, 1) = 1.f / (float)(tan(myFov / 2.f));
	myProjectionMatrix(2, 2) = ((float)myResolution.x / (float)myResolution.y) * (float)(1.f / (tan(myFov / 2.f)));
	myProjectionMatrix(3, 3) = myFarPlane / (myFarPlane - myNearPlane);
	myProjectionMatrix(3, 4) = 1.f;
	myProjectionMatrix(4, 3) = (-myNearPlane * myFarPlane) / (myFarPlane - myNearPlane);
	myProjectionMatrix(4, 4) = 0;
}

CU::Vec3f ISTE::EditorCamera::GetPosition()
{
	return { myTransform(4,1), myTransform(4,2) ,myTransform(4,3) };
}

CU::Matrix4x4f ISTE::EditorCamera::GetWorldToClip()
{
	return CU::Matrix4x4f::FastInverse(myTransform) * myProjectionMatrix;
}

void ISTE::EditorCamera::Move(float /*aTimeDelta*/)
{
	CU::Vec3f right = { cosf(-myHeadYaw), 0.f ,sinf(-myHeadYaw) };
	CU::Vec3f forward = { sinf(myHeadYaw), 0.f ,cosf(myHeadYaw) };

	//movement in z
	if (myIH->IsKeyHeldDown('W'))
	{
		myTransform.GetRow(4).x += forward.x;
		myTransform.GetRow(4).z += forward.z;
	}
	if (myIH->IsKeyHeldDown('S'))
	{
		myTransform.GetRow(4).x += -forward.x;
		myTransform.GetRow(4).z += -forward.z;
	}

	//movement in x
	if (myIH->IsKeyHeldDown('D'))
	{
		myTransform.GetRow(4).x += right.x;
		myTransform.GetRow(4).z += right.z;
	}
	if (myIH->IsKeyHeldDown('A'))
	{
		myTransform.GetRow(4).x += -right.x;
		myTransform.GetRow(4).z += -right.z;
	}

	//movement in y
	if (myIH->IsKeyHeldDown(VK_SPACE))
		myTransform(4, 2) += myMovementSpeed;
	if (myIH->IsKeyHeldDown(VK_SHIFT))
		myTransform(4, 2) -= myMovementSpeed;
}

void ISTE::EditorCamera::LookAround(float /*aTimeDelta*/)
{
	if (!myIH->IsKeyHeldDown(MK_RBUTTON))
		return;

	POINT mVel = myIH->GetMouseVelocity();

	//transform directions
	auto& right = myTransform.GetRow(1);
	auto& up = myTransform.GetRow(2);
	auto& forward = myTransform.GetRow(3);


	float pitchAmount = mVel.y * myLookAroundSpeed;
	float yawAmount   = mVel.x * myLookAroundSpeed;


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

void ISTE::EditorCamera::OrhoNormalization()
{
	//this is honestly really ugly 
	//should make it so that you can turn vec4s in to vec3 just by the operator=

	CU::Vec3f right = { myTransform(1,1), myTransform(1,2),myTransform(1,3) };
	CU::Vec3f up = { myTransform(2,1), myTransform(2,2),myTransform(2,3) };
	CU::Vec3f forward = { myTransform(3,1), myTransform(3,2),myTransform(3,3) };

	forward.Normalize();
	up = forward.Cross(right).GetNormalized();
	right = up.Cross(forward).GetNormalized();

	myTransform(1, 1) = right.x;
	myTransform(1, 2) = right.y;
	myTransform(1, 3) = right.z;

	myTransform(2, 1) = up.x;
	myTransform(2, 2) = up.y;
	myTransform(2, 3) = up.z;

	myTransform(3, 1) = forward.x;
	myTransform(3, 2) = forward.y;
	myTransform(3, 3) = forward.z;
}