#define _USE_MATH_DEFINES
#include "Euler.h"
#include "Math.h"

//Sets a rotation in degrees
void CU::Euler::SetRotation(Vec3f aXYZRotation)
{
    myAngles = aXYZRotation * ISTE::DegToRad;
    AnglesUpdated();
    myQuaternion = Quaternionf(myAngles);
}

void CU::Euler::SetRotation(float aX, float aY, float aZ)
{
    SetRotation({ aX, aY, aZ });
}

//Adds a rotation in degrees
void CU::Euler::AddRotation(Vec3f aXYZRotation)
{
    myAngles +=  aXYZRotation * ISTE::DegToRad;
    AnglesUpdated(); 
    myQuaternion *= Quaternionf(aXYZRotation * ISTE::DegToRad);
}

void CU::Euler::AddRotation(float aX, float aY, float aZ)
{
    AddRotation({ aX, aY, aZ });
}

//Adds a rotation in degrees
CU::Vec3f CU::Euler::GetAngles()
{
    return myAngles * ISTE::RadToDeg;
}

CU::Matrix4x4f CU::Euler::GetRotationMatrix()
{
    return myQuaternion.GetRotationMatrix4X4();
}

void CU::Euler::AnglesUpdated()
{
    if (myAngles.x > 360.f)
        myAngles.x = 0;
    if (myAngles.y > 360.f)
        myAngles.y = 0;
    if (myAngles.z > 360.f)
        myAngles.z = 0;
}

void CU::Euler::ToEulerAngles() 
{
    // roll (x-axis rotation)
    double sinr_cosp = 2 * (myQuaternion.w * myQuaternion.x + myQuaternion.y * myQuaternion.z);
    double cosr_cosp = 1 - 2 * (myQuaternion.x * myQuaternion.x + myQuaternion.y * myQuaternion.y);
    myAngles.x = std::atan2f(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (myQuaternion.w * myQuaternion.y - myQuaternion.z * myQuaternion.x);
    if (std::abs(sinp) >= 1)
        myAngles.y = std::copysignf(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        myAngles.y = std::asinf(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (myQuaternion.w * myQuaternion.z + myQuaternion.x * myQuaternion.y);
    double cosy_cosp = 1 - 2 * (myQuaternion.y * myQuaternion.y + myQuaternion.z * myQuaternion.z);
    myAngles.z = std::atan2f(siny_cosp, cosy_cosp);
}
