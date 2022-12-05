#include "Camera.h"

#include <ISTE/Math/Math.h>

ISTE::Camera::Camera()
{
	myTransform.GetTranslationV3() = CU::Vec3f(-10, 2, 0);
}

ISTE::Camera::~Camera()
{
}

void ISTE::Camera::SetPerspectiveProjection(const float aHorizontalFovInDeg, const CU::Vec2Ui& aRes, const float aNearPlane, const float aFarPlane)
{
	myFov = aHorizontalFovInDeg;
	myResolution = aRes;
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;

	myProjectionMatrix(1, 1) = 1.f / (float)(tan(myFov * ISTE::DegToRad / 2.f));
	myProjectionMatrix(2, 2) = ((float)myResolution.x / (float)myResolution.y) * (float)(1.f / (tan(myFov * ISTE::DegToRad / 2.f)));
	myProjectionMatrix(3, 3) = myFarPlane / (myFarPlane - myNearPlane);
	myProjectionMatrix(3, 4) = 1.f;
	myProjectionMatrix(4, 3) = (-myNearPlane * myFarPlane) / (myFarPlane - myNearPlane);
	myProjectionMatrix(4, 4) = 0;

	//float calc = 2.f * tan((myFov * ISTE::DegToRad) / 2.f);
	//float ratio = (float)myResolution.x / (float)myResolution.y;

	//float hNear = calc * myNearPlane;
	//float wNear = hNear * ratio;
	//float hFar = calc * myFarPlane;
	//float wFar = hFar * ratio;

	//CU::Vec3f cPos = myTransform.GetPos();
	//CU::Vec3f dir = myTransform.GetForward();

	//CU::Vec3f fC = cPos + dir * myFarPlane;
	//CU::Vec3f nC = cPos + dir * myNearPlane;

	//CU::Vec4f tempUp = myTransform.GetRow(2);
	//CU::Vec3f up = { tempUp.x,tempUp.y ,tempUp.z };

	//CU::Vec4f tempRight = myTransform.GetRow(1);
	//CU::Vec3f right = { tempRight.x,tempRight.y ,tempRight.z };

	////points
	//CU::Vec3f fTL = fC + (up * hFar / 2.f) - (right * wFar / 2.f);
	//CU::Vec3f fTR = fC + (up * hFar / 2.f) + (right * wFar / 2.f);
	//CU::Vec3f fBL = fC - (up * hFar / 2.f) - (right * wFar / 2.f);
	//CU::Vec3f fBR = fC - (up * hFar / 2.f) + (right * wFar / 2.f);

	//CU::Vec3f nTL = nC + (up * hNear / 2.f) - (right * wNear / 2.f);
	//CU::Vec3f nTR = nC + (up * hNear / 2.f) + (right * wNear / 2.f);
	//CU::Vec3f nBL = nC - (up * hNear / 2.f) - (right * wNear / 2.f);
	//CU::Vec3f nBR = nC - (up * hNear / 2.f) + (right * wNear / 2.f);


	//myFrustum.myPlanes[0].InitWith3Points(nTR, fTR, nBR);

	//myFrustum.myPlanes[1].InitWith3Points(nBL, fBL, nTL);

	//myFrustum.myPlanes[2].InitWith3Points(nTL, fTL, nTR);

	//myFrustum.myPlanes[3].InitWith3Points(nBR, fBR, nBL);

	//myFrustum.myPlanes[4].InitWith3Points(nTR, nBR, nTL);

	//myFrustum.myPlanes[5].InitWith3Points(fTL, fBL, fTR);

}

void ISTE::Camera::SetFov(const float aHorizontalFovInDeg)
{
	SetPerspectiveProjection(aHorizontalFovInDeg, myResolution, myNearPlane, myFarPlane);
}

const CU::Vec3f ISTE::Camera::GetPosition() const
{
	return CU::Vec3f(myTransform(4, 1), myTransform(4, 2), myTransform(4, 3));
}

const CU::Matrix4x4f ISTE::Camera::GetWorldToClip() const
{
	return CU::Matrix4x4f::FastInverse(myTransform) * myProjectionMatrix;
}

ISTE::Frustum ISTE::Camera::GetFrustum()
{

	//test, the frustum should be created in setprojection and only updated here
	float calc = 2.f * tan((myFov * ISTE::DegToRad) / 2.f);
	float ratio = (float)myResolution.x / (float)myResolution.y;

	float hNear = calc * myNearPlane;
	float wNear = hNear * ratio;
	float hFar = calc * myFarPlane;
	float wFar = hFar * ratio;

	CU::Vec3f cPos = myTransform.GetTranslationV3();
	CU::Vec3f dir = myTransform.GetForwardV3();

	CU::Vec3f fC = cPos + dir * myFarPlane;
	CU::Vec3f nC = cPos + dir * myNearPlane;

	CU::Vec4f tempUp = myTransform.GetRow(2);
	CU::Vec3f up = { tempUp.x,tempUp.y ,tempUp.z };

	CU::Vec4f tempRight = myTransform.GetRow(1);
	CU::Vec3f right = { tempRight.x,tempRight.y ,tempRight.z };

	//points
	CU::Vec3f fTL = fC + (up * hFar / 2.f) - (right * wFar / 2.f);
	CU::Vec3f fTR = fC + (up * hFar / 2.f) + (right * wFar / 2.f);
	CU::Vec3f fBL = fC - (up * hFar / 2.f) - (right * wFar / 2.f);
	CU::Vec3f fBR = fC - (up * hFar / 2.f) + (right * wFar / 2.f);

	CU::Vec3f nTL = nC + (up * hNear / 2.f) - (right * wNear / 2.f);
	CU::Vec3f nTR = nC + (up * hNear / 2.f) + (right * wNear / 2.f);
	CU::Vec3f nBL = nC - (up * hNear / 2.f) - (right * wNear / 2.f);
	CU::Vec3f nBR = nC - (up * hNear / 2.f) + (right * wNear / 2.f);


	myFrustum.myPlanes[0].InitWith3Points(nTR, fTR, nBR);

	myFrustum.myPlanes[1].InitWith3Points(nBL, fBL, nTL);

	myFrustum.myPlanes[2].InitWith3Points(nTL, fTL, nTR);

	myFrustum.myPlanes[3].InitWith3Points(nBR, fBR, nBL);

	myFrustum.myPlanes[4].InitWith3Points(nTR, nBR, nTL);

	myFrustum.myPlanes[5].InitWith3Points(fTL, fBL, fTR);

	return myFrustum;
}
