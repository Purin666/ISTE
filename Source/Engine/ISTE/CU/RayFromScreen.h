#pragma once

#include "ISTE/Math/Vec.h"
#include <Windows.h>

#include "ISTE/Graphics/Camera.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Math/Vec3.h"

namespace ISTE
{
    class ScreenHelper
    {
    public:
        struct GPUPickedData
        {
            EntityID myEntityId;
            CU::Vec3f myPosition;
        };

    public:
        struct Ray
        {
            CU::Vec3f myDir;
            CU::Vec3f myOrigin;
        };

        POINT GetMouseRelativePosition(bool nullIfNegative = false)
        {
            POINT mP = {};

            GetCursorPos(&mP);

            HWND hwnd = Context::Get()->myWindow->GetWindowsHandle();
            
            ScreenToClient(hwnd, &mP);
            if (nullIfNegative)
            {
                if (mP.x < 0) mP.x = 0;
                if (mP.y < 0) mP.y = 0;
            }

            return mP;
        }

        Ray GetRayFromPoint(POINT aPoint, const unsigned int* aWSize)
        {

            Ray ray;

            Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();

            CU::Vec4f dir;

            float xD = 2 * (aPoint.x / (float)aWSize[0] - 0.5f);
            float yD = 2 * (aPoint.y / (float)aWSize[1] - 0.5f);
            yD *= -1;

            CU::Matrix4x4f comb = CU::Matrix4x4f::Inverse(camera.GetProjectionMatrix()) * camera.GetTransformNonConst();
            dir = camera.GetTransformNonConst().GetRow(3) + xD * comb.GetRow(1) + yD * comb.GetRow(2);
            ray.myOrigin = camera.GetPosition();
            ray.myDir = CU::Vec3f(dir.x, dir.y, dir.z);


            return ray;
        }

        GPUPickedData Pick(POINT aPoint)
        {  
            auto data = Context::Get()->myGraphicsEngine->Pick({ (unsigned int)aPoint.x, (unsigned int)aPoint.y });
            CU::Vec2Ui res = Context::Get()->myDX11->GetBackBufferRes();
     
            const CU::Matrix4x4f& invClipSpaceMatrix = CU::Matrix4x4f::Inverse(Context::Get()->myGraphicsEngine->GetCamera().GetProjectionMatrix());
            const CU::Matrix4x4f& viewMatrix = Context::Get()->myGraphicsEngine->GetCamera().GetTransform();

            CU::Vec2f  uv = { (float)aPoint.x / (float)res.x , (float)aPoint.y / (float)res.y };
            uv *= 2.f;
            uv.x -= 1;
            uv.y -= 1;

            uv.y *= -1.f; 

            CU::Vec4f pos = { uv.x, uv.y , data.myDepth, 1 };

            pos = pos * invClipSpaceMatrix;
            pos /= pos.w;
            pos = pos * viewMatrix;

            return{ (EntityID)data.myId, {pos.x, pos.y, pos.z} };
        }

    };


}