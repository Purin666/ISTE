#include "TriggerSystem.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Context.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/Resources/ModelManager.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/DebugDrawer.h"

namespace ISTE
{
	void ISTE::TriggerSystem::Check()
	{
		BoxBox();
		BoxSphere();
		SphereSphere();
	}
	void TriggerSystem::BoxBox()
	{
		ComponentPool& triggerPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TriggerComponent>();
		ComponentPool& transformPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		ComponentPool& modelPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<ModelComponent>();

		std::set<EntityID>& ids = myEntities.at((int)MapValues::Box);

		//ugly but dont care
		std::vector<EntityID> vectorIds;
		vectorIds.reserve(ids.size() + 1);

		for (auto& i : ids)
		{
			vectorIds.emplace_back(i);
		}

		for (int i = 0; i < (int)vectorIds.size() - 1; i++)
		{
			EntityID id = vectorIds[i];
			EntityIndex index = GetEntityIndex(id);

			CU::Vec3f tingApp(1, 1, 1);

			TransformComponent* transformComp = (TransformComponent*)transformPool.Get(index);
			TriggerComponent* triggerComp = (TriggerComponent*)triggerPool.Get(index);
			ModelComponent* mComp = (ModelComponent*)modelPool.Get(index);

			if (mComp != nullptr)
			{
				tingApp = Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMax - Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMin;
			}

			CU::AABB3D<float> box;

			CU::Vec3f midPoint = transformComp->myPosition + triggerComp->myOffset;
			CU::Vec3f size = ((transformComp->myScale * tingApp) * triggerComp->mySize) / 2.0f;

			box.InitWithMinAndMax((midPoint - size), (midPoint + size));

			for (int c = i + 1; c < vectorIds.size(); c++)
			{
				EntityID otherId = vectorIds[c];
				EntityIndex otherIndex = GetEntityIndex(otherId);

				CU::Vec3f otherApp(1, 1, 1);

				TriggerComponent* otherTriggerComp = (TriggerComponent*)triggerPool.Get(otherIndex);
				TransformComponent* otherTransformComp = (TransformComponent*)transformPool.Get(otherIndex);
				ModelComponent* otherMComp = (ModelComponent*)modelPool.Get(otherIndex);

				if (otherMComp != nullptr)
				{
					otherApp = Context::Get()->myModelManager->GetModel(otherMComp->myModelId)->myAABB.myMax - Context::Get()->myModelManager->GetModel(otherMComp->myModelId)->myAABB.myMin;
				}


				CU::AABB3D<float> otherBox;
				
				midPoint = otherTransformComp->myPosition + otherTriggerComp->myOffset;
				size = ((otherTransformComp->myScale * otherApp) * otherTriggerComp->mySize) / 2.0f;

				otherBox.InitWithMinAndMax((midPoint - size), (midPoint + size));

				if (CU::IsInside(box, otherBox))
				{
					for (auto& b : Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourHandle(id).GetEverything())
					{
						b->OnTrigger(otherId);
					}

					for (auto& b : Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourHandle(otherId).GetEverything())
					{
						b->OnTrigger(id);
					}
				}
			}
		}
	}
	void TriggerSystem::BoxSphere()
	{
		ComponentPool& triggerPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TriggerComponent>();
		ComponentPool& sphereTriggerPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<SphereTriggerComponent>();
		ComponentPool& transformPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		ComponentPool& modelPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<ModelComponent>();

		std::set<EntityID>& boxIds = myEntities.at((int)MapValues::Box);
		std::set<EntityID>& sphereIds = myEntities.at((int)MapValues::Sphere);

		for (auto& boxID : boxIds)
		{
			EntityIndex boxIndex = GetEntityIndex(boxID);

			TransformComponent* boxTransformComponent = (TransformComponent*)transformPool.Get(boxIndex);
			TriggerComponent* boxTriggerComponent = (TriggerComponent*)triggerPool.Get(boxIndex);
			ModelComponent* boxModelComponent = (ModelComponent*)modelPool.Get(boxIndex);

			CU::Vec3f boxApp = { 1,1,1 };

			if (boxModelComponent != nullptr)
			{
				boxApp = Context::Get()->myModelManager->GetModel(boxModelComponent->myModelId)->myAABB.myMax - Context::Get()->myModelManager->GetModel(boxModelComponent->myModelId)->myAABB.myMin;
			}

			CU::AABB3D<float> box;

			CU::Vec3f midPoint = boxTransformComponent->myPosition + boxTriggerComponent->myOffset;
			CU::Vec3f size = ((boxTransformComponent->myScale * boxApp) * boxTriggerComponent->mySize) / 2.0f;

			box.InitWithMinAndMax((midPoint - size), (midPoint + size));


			for (auto& sphereID : sphereIds)
			{
				EntityIndex sphereIndex = GetEntityIndex(sphereID);

				TransformComponent* sphereTransformComponent = (TransformComponent*)transformPool.Get(sphereIndex);
				SphereTriggerComponent* sphereTriggerComponent = (SphereTriggerComponent*)sphereTriggerPool.Get(sphereIndex);
				ModelComponent* sphereModelComponent = (ModelComponent*)modelPool.Get(sphereIndex);

				float sphereApp = 1;


				if (sphereModelComponent != nullptr)
				{
					sphereApp = Context::Get()->myModelManager->GetModel(sphereModelComponent->myModelId)->myAABB.myMax.x - Context::Get()->myModelManager->GetModel(sphereModelComponent->myModelId)->myAABB.myMin.x;
				}

				CU::Sphere<float> sphere;

				midPoint = sphereTransformComponent->myPosition + sphereTriggerComponent->myOffset;
				float fsize = ((sphereTransformComponent->myScale.x * sphereApp) * sphereTriggerComponent->myRadius);

				sphere.InitWithCenterAndRadius(midPoint, fsize);

				if (CU::IsInside(box, sphere))
				{
					for (auto& b : Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourHandle(boxID).GetEverything())
					{
						b->OnTrigger(sphereID);
					}

					for (auto& b : Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourHandle(sphereID).GetEverything())
					{
						b->OnTrigger(boxID);
					}
				}

			}
		}

	}
	void TriggerSystem::SphereSphere()
	{
		ComponentPool& sphereTriggerPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<SphereTriggerComponent>();
		ComponentPool& transformPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		ComponentPool& modelPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<ModelComponent>();

		std::set<EntityID>& ids = myEntities.at((int)MapValues::Sphere);

		//ugly but dont care
		std::vector<EntityID> vectorIds;
		vectorIds.reserve(ids.size() + 1);

		for (auto& i : ids)
		{
			vectorIds.emplace_back(i);
		}

		for (int i = 0; i < (int)vectorIds.size() - 1; i++)
		{
			EntityID id = vectorIds[i];
			EntityIndex index = GetEntityIndex(id);

			float tingApp = 1;

			TransformComponent* transformComp = (TransformComponent*)transformPool.Get(index);
			SphereTriggerComponent* triggerComp = (SphereTriggerComponent*)sphereTriggerPool.Get(index);
			ModelComponent* mComp = (ModelComponent*)modelPool.Get(index);
			

			if (mComp != nullptr)
			{
				tingApp = Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMax.x - Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMin.x;
			}

			CU::Sphere<float> sphere;

			CU::Vec3f midPoint = transformComp->myPosition + triggerComp->myOffset;
			float size = ((transformComp->myScale.x * tingApp) * triggerComp->myRadius);

			sphere.InitWithCenterAndRadius(midPoint, size);

			for (int c = i + 1; c < vectorIds.size(); c++)
			{
				EntityID otherId = vectorIds[c];
				EntityIndex otherIndex = GetEntityIndex(otherId);

				float otherApp = 1;

				SphereTriggerComponent* otherTriggerComp = (SphereTriggerComponent*)sphereTriggerPool.Get(otherIndex);
				TransformComponent* otherTransformComp = (TransformComponent*)transformPool.Get(otherIndex);
				ModelComponent* otherMComp = (ModelComponent*)modelPool.Get(otherIndex);

				if (otherMComp != nullptr)
				{
					otherApp = Context::Get()->myModelManager->GetModel(otherMComp->myModelId)->myAABB.myMax.x - Context::Get()->myModelManager->GetModel(otherMComp->myModelId)->myAABB.myMin.x;
				}

				CU::Sphere<float> otherSphere;

				midPoint = otherTransformComp->myPosition + otherTriggerComp->myOffset;
				size = ((otherTransformComp->myScale.x * otherApp) * otherTriggerComp->myRadius);

				otherSphere.InitWithCenterAndRadius(midPoint, size);

				if (CU::IsInside(sphere, otherSphere))
				{
					for (auto& b : Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourHandle(id).GetEverything())
					{
						b->OnTrigger(otherId);
					}

					for (auto& b : Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourHandle(otherId).GetEverything())
					{
						b->OnTrigger(id);
					}
				}
			}
		}
	}

	void TriggerSystem::DrawBoxes()
	{
		ComponentPool& triggerPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TriggerComponent>();
		ComponentPool& transformPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		ComponentPool& modelPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<ModelComponent>();

		std::set<EntityID>& ids = myEntities.at((int)MapValues::Box);

		for (auto& id : ids)
		{
			EntityIndex index = GetEntityIndex(id);

			CU::Vec3f tingApp(1, 1, 1);

			TransformComponent* transformComp = (TransformComponent*)transformPool.Get(index);
			TriggerComponent* triggerComp = (TriggerComponent*)triggerPool.Get(index);
			ModelComponent* mComp = (ModelComponent*)modelPool.Get(index);

			if (mComp != nullptr)
			{
				tingApp = Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMax - Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMin;
			}

			CU::AABB3D<float> box;

			CU::Vec3f midPoint = transformComp->myPosition + triggerComp->myOffset;
			CU::Vec3f size = ((transformComp->myScale * tingApp) * triggerComp->mySize) / 2.0f;

			box.InitWithMinAndMax((midPoint - size), (midPoint + size));

			DrawBox(box);
		}
	}
	void TriggerSystem::DrawSpheres()
	{
		ComponentPool& sphereTriggerPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<SphereTriggerComponent>();
		ComponentPool& transformPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		ComponentPool& modelPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<ModelComponent>();

		std::set<EntityID>& ids = myEntities.at((int)MapValues::Sphere);

		for (auto& id : ids)
		{
			EntityIndex index = GetEntityIndex(id);

			float tingApp = 1;

			TransformComponent* transformComp = (TransformComponent*)transformPool.Get(index);
			SphereTriggerComponent* triggerComp = (SphereTriggerComponent*)sphereTriggerPool.Get(index);
			ModelComponent* mComp = (ModelComponent*)modelPool.Get(index);


			if (mComp != nullptr)
			{
				tingApp = Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMax.x - Context::Get()->myModelManager->GetModel(mComp->myModelId)->myAABB.myMin.x;
			}

			CU::Sphere<float> sphere;

			CU::Vec3f midPoint = transformComp->myPosition + triggerComp->myOffset;
			float size = ((transformComp->myScale.x * tingApp) * triggerComp->myRadius);

			sphere.InitWithCenterAndRadius(midPoint, size);

			DrawSphere(sphere);
		}
	}

	void TriggerSystem::DrawBox(const CU::AABB3D<float>& aBox)
	{
		LineCommand line;
		line.myColor = { 0,10,0 };

		DebugDrawer& drawer = ISTE::Context::Get()->myGraphicsEngine->GetDebugDrawer();

		line.myFromPosition = aBox.min;
		line.myToPosition = aBox.min;
		line.myToPosition.z = aBox.max.z;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aBox.min;
		line.myToPosition.x = aBox.max.x;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aBox.min;
		line.myToPosition.y = aBox.max.y;
		drawer.AddDynamicLineCommand(line);

		line.myFromPosition = aBox.max;
		line.myToPosition = aBox.max;
		line.myToPosition.z = aBox.min.z;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aBox.max;
		line.myToPosition.x = aBox.min.x;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aBox.max;
		line.myToPosition.y = aBox.min.y;
		drawer.AddDynamicLineCommand(line);

		line.myFromPosition = aBox.min;
		line.myFromPosition.x = aBox.max.x;
		line.myToPosition = aBox.max;
		line.myToPosition.y = aBox.min.y;
		drawer.AddDynamicLineCommand(line);
		line.myFromPosition = aBox.min;
		line.myFromPosition.z = aBox.max.z;
		line.myToPosition = aBox.max;
		line.myToPosition.y = aBox.min.y;
		drawer.AddDynamicLineCommand(line);

		line.myFromPosition = aBox.max;
		line.myFromPosition.x = aBox.min.x;
		line.myToPosition = aBox.min;
		line.myToPosition.y = aBox.max.y;
		drawer.AddDynamicLineCommand(line);
		line.myFromPosition = aBox.max;
		line.myFromPosition.z = aBox.min.z;
		line.myToPosition = aBox.min;
		line.myToPosition.y = aBox.max.y;
		drawer.AddDynamicLineCommand(line);


		line.myFromPosition = aBox.min;
		line.myFromPosition.x = aBox.max.x;
		line.myToPosition = aBox.max;
		line.myToPosition.z = aBox.min.z;
		drawer.AddDynamicLineCommand(line);
		line.myFromPosition = aBox.min;
		line.myFromPosition.z = aBox.max.z;
		line.myToPosition = aBox.max;
		line.myToPosition.x = aBox.min.x;
		drawer.AddDynamicLineCommand(line);


	}
	void TriggerSystem::DrawSphere(const CU::Sphere<float>& aSphere)
	{
		LineCommand line;
		line.myColor = { 0,10,0 };

		DebugDrawer& drawer = ISTE::Context::Get()->myGraphicsEngine->GetDebugDrawer();

		line.myFromPosition = aSphere.center;

		line.myToPosition = aSphere.center;
		line.myToPosition.y -= aSphere.radius;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aSphere.center;
		line.myToPosition.y += aSphere.radius;
		drawer.AddDynamicLineCommand(line);

		line.myToPosition = aSphere.center;
		line.myToPosition.x -= aSphere.radius;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aSphere.center;
		line.myToPosition.x += aSphere.radius;
		drawer.AddDynamicLineCommand(line);

		line.myToPosition = aSphere.center;
		line.myToPosition.z -= aSphere.radius;
		drawer.AddDynamicLineCommand(line);
		line.myToPosition = aSphere.center;
		line.myToPosition.z += aSphere.radius;
		drawer.AddDynamicLineCommand(line);

		Draw3DSphere(aSphere);
	}
	void TriggerSystem::Draw3DSphere(const CU::Sphere<float>& aSphere, const size_t aRes)
	{
		LineCommand r; r.myColor = { 10.f,  0.f,  0.f};
		LineCommand g; g.myColor = {  0.f, 10.f,  0.f };
		LineCommand b; b.myColor = {  0.f,  0.f, 10.f };

		std::vector<CU::Vec3f> xy, yz, zx;
		xy.resize(aRes);
		yz.resize(aRes);
		zx.resize(aRes);

		size_t i;
		CU::Vec2f v;
		const float mod = float(360 / aRes);

		// calc
		for (i = 0; i < aRes; ++i)
		{
			v = CU::GetVectorFromRadians((float)i * mod * ISTE::DegToRad) * aSphere.radius;
			
			xy[i].x = v.x + aSphere.center.x;
			xy[i].y = v.y + aSphere.center.y;
			xy[i].z = aSphere.center.z;

			yz[i].x = aSphere.center.x;
			yz[i].y = v.x + aSphere.center.y;
			yz[i].z = v.y + aSphere.center.z;

			zx[i].x = v.y + aSphere.center.x;
			zx[i].y = aSphere.center.y;
			zx[i].z = v.x + aSphere.center.z;
		}

		// draw
		DebugDrawer& drawer = ISTE::Context::Get()->myGraphicsEngine->GetDebugDrawer();
		for (i = 0; i < aRes-1; ++i)
		{
			r.myFromPosition = yz[i];
			r.myToPosition = yz[i+1];
			drawer.AddDynamicLineCommand(r);

			g.myFromPosition = zx[i];
			g.myToPosition = zx[i+1];
			drawer.AddDynamicLineCommand(g);

			b.myFromPosition = xy[i];
			b.myToPosition = xy[i+1];
			drawer.AddDynamicLineCommand(b);
		}
		r.myFromPosition = yz.back();
		r.myToPosition = yz[0];
		drawer.AddDynamicLineCommand(r);

		g.myFromPosition = zx.back();
		g.myToPosition = zx[0];
		drawer.AddDynamicLineCommand(g);

		b.myFromPosition = xy.back();
		b.myToPosition = xy[0];
		drawer.AddDynamicLineCommand(b);
	}
}
