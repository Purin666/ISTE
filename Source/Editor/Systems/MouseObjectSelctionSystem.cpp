#include "MouseObjectSelctionSystem.h"

#include "ISTE/Context.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/Model.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/Camera.h"
#include "ISTE/Graphics/DX11.h"

#include "ISTE/Math/Math.h"

namespace ISTE
{
    bool MouseObjectSelectionSystem::IsInside(Box aBox, CU::Vec3f aPosition)
    {
        //kinda ugly
        if (((aPosition.x >= aBox.myMin.x) && (aPosition.x <= aBox.myMax.x)) && ((aPosition.y >= aBox.myMin.y) && (aPosition.y <= aBox.myMax.y)) && ((aPosition.z >= aBox.myMin.z) && (aPosition.z <= aBox.myMax.z)))
        {
            return true;
        }

        return false;
    }

    bool MouseObjectSelectionSystem::Intersects(Box aBox, ScreenHelper::Ray aRay, float& aDist)
    {
        if (IsInside(aBox, aRay.myOrigin))
        {
            aDist = 0;
            return true;
        }
        CU::Vec3f a = aBox.myMin - aRay.myOrigin;
        CU::Vec3f b = aBox.myMax - aRay.myOrigin;
        CU::Vec3f tMin =  CU::Vec3f(a.x / aRay.myDir.x, a.y / aRay.myDir.y, a.z / aRay.myDir.z);
        CU::Vec3f tMax = CU::Vec3f(b.x / aRay.myDir.x, b.y / aRay.myDir.y, b.z / aRay.myDir.z);

        CU::Vec3f t1 = CU::Vec3f(min(tMin.x, tMax.x), min(tMin.y, tMax.y), min(tMin.z, tMax.z));
        CU::Vec3f t2 = CU::Vec3f(max(tMin.x, tMax.x), max(tMin.y, tMax.y), max(tMin.z, tMax.z));
        float tNear = max(max(t1.x, t1.y), t1.z);
        float tFar = min(min(t2.x, t2.y), t2.z);

        if (tNear > tFar)
            return false;

        aDist = tNear;

        return true;
    }

    EntityID ISTE::MouseObjectSelectionSystem::GetObjectClickedOn()
    {
        Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();

        EntityID retId = INVALID_ENTITY;

        unsigned int wS[2] = { Context::Get()->myDX11->GetBackBufferRes().x,Context::Get()->myDX11->GetBackBufferRes().y };

        POINT mP = helper.GetMouseRelativePosition();



        ScreenHelper::Ray ray = helper.GetRayFromPoint(mP, wS);

        ComponentPool& tPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
        //get model pool

        std::vector<EntityID> hitIds;
        std::vector<float> hitPos;

        for (auto& id : myEntities[0])
        {
            Box box;

            TransformComponent* transform = (TransformComponent*)tPool.Get(GetEntityIndex(id));

            box.myMin = transform->myPosition - (transform->myScale / 2.0f);
            box.myMax = transform->myPosition + (transform->myScale / 2.0f);
            box.mySize = transform->myScale;

            float dist = 0;

            if (Intersects(box, ray, dist))
            {

                hitIds.push_back(id);
                hitPos.push_back(dist);
            }

        }



        if (hitPos.size() != 0)
        {
            retId = hitIds[0];
            float length = hitPos[0];

            for (int i = 1; i < hitPos.size(); i++)
            {
                if (hitPos[i] < length)
                {
                    retId = hitIds[i];
                    length = hitPos[i];
                }
            }

        }
        

        return retId;
    }
}
