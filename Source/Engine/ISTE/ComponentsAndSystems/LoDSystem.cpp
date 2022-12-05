#include "LoDSystem.h"

#include "ISTE/Events/EventHandler.h"
#include "ISTE/Graphics/GraphicsEngine.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"

#include "ISTE/Graphics/Resources/Model.h"
#include "ISTE/Graphics/Resources/ModelManager.h"

#include <queue>
#include <deque>

//againstPlayer

#include "ISTE/Math/Matrix4x4.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	struct LoDLine
	{
		~LoDLine() = default;
		CU::Vec3f myFrom;
		CU::Vec3f myTo;
	};

	struct LoDBox
	{
		~LoDBox() = default;
		CU::Vec3f myMin;
		CU::Vec3f myMax;
	};


	bool GetIntersection(float fDst1, float fDst2, const CU::Vec3f& P1, const CU::Vec3f& P2, CU::Vec3f& Hit) {
		if ((fDst1 * fDst2) >= 0.0f) return 0;
		if (fDst1 == fDst2) return 0;
		Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
		return 1;
	}

	bool InBox(CU::Vec3f Hit, CU::Vec3f B1, CU::Vec3f B2, const int Axis) {
		if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
		if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
		if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
		return 0;
	}

	bool IsIntersecting(const LoDLine& aLine, const LoDBox& aBox)
	{
		if (aLine.myTo.x < aBox.myMin.x && aLine.myFrom.x < aBox.myMin.x) return false;
		if (aLine.myTo.x > aBox.myMax.x && aLine.myFrom.x > aBox.myMax.x) return false;
		if (aLine.myTo.y < aBox.myMin.y && aLine.myFrom.y < aBox.myMin.y) return false;
		if (aLine.myTo.y > aBox.myMax.y && aLine.myFrom.y > aBox.myMax.y) return false;
		if (aLine.myTo.z < aBox.myMin.z && aLine.myFrom.z < aBox.myMin.z) return false;
		if (aLine.myTo.z > aBox.myMax.z && aLine.myFrom.z > aBox.myMax.z) return false;

		if (aLine.myFrom.x > aBox.myMin.x && aLine.myFrom.x < aBox.myMax.x &&
			aLine.myFrom.y > aBox.myMin.y && aLine.myFrom.y < aBox.myMax.y &&
			aLine.myFrom.z > aBox.myMin.z && aLine.myFrom.z < aBox.myMax.z)
		{
			//Hit = L1;
			return true;
		}

		CU::Vec3f hit;

		if  (  (GetIntersection(aLine.myFrom.x - aLine.myTo.x, aLine.myTo.x - aLine.myTo.x, aLine.myFrom, aLine.myTo,hit) && InBox(hit, aBox.myMin, aBox.myMax, 1))
			|| (GetIntersection(aLine.myFrom.y - aLine.myTo.y, aLine.myTo.y - aLine.myTo.y, aLine.myFrom, aLine.myTo,hit) && InBox(hit, aBox.myMin, aBox.myMax, 2))
			|| (GetIntersection(aLine.myFrom.z - aLine.myTo.z, aLine.myTo.z - aLine.myTo.z, aLine.myFrom, aLine.myTo,hit) && InBox(hit, aBox.myMin, aBox.myMax, 3))
			|| (GetIntersection(aLine.myFrom.x - aBox.myMax.x, aLine.myTo.x - aBox.myMax.x, aLine.myFrom, aLine.myTo,hit) && InBox(hit, aBox.myMin, aBox.myMax, 1))
			|| (GetIntersection(aLine.myFrom.y - aBox.myMax.y, aLine.myTo.y - aBox.myMax.y, aLine.myFrom, aLine.myTo,hit) && InBox(hit, aBox.myMin, aBox.myMax, 2))
			|| (GetIntersection(aLine.myFrom.z - aBox.myMax.z, aLine.myTo.z - aBox.myMax.z, aLine.myFrom, aLine.myTo,hit) && InBox(hit, aBox.myMin, aBox.myMax, 3)))
			return true;

		return false;
	}

	void LoDSystem::Update()
	{
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
		ComponentPool& loDPool = scene.GetComponentPool<LoDComponent>();

		Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();

		struct EventData
		{
			EntityID entity;
			LODLEVEL lodLevel;
		};

		std::queue<EventData> events;

		for (auto& entityID : myEntities[0])
		{
			EntityIndex index = GetEntityIndex(entityID);

			TransformComponent* eTransform = (TransformComponent*)transformPool.Get(index);
			LoDComponent* lodComp = (LoDComponent*)loDPool.Get(index);

			//where percentage check vs dist check should happen here

			//dist check
			float entityDist = (camera.GetPosition() - eTransform->myPosition).LengthSqr();
			for (LODLEVEL i = 0; i < MAX_LODLEVEL; i++)
			{
				if (entityDist < lodComp->myLoDLevels[i])
				{
					events.push({ entityID, i });
					break;
				}
			}
		}



		while (!events.empty())
		{
			EventData& data = events.front();
			for (auto& [identifier, callback] : myCallBacks)
			{
				callback(data.entity, data.lodLevel);
			}

			events.pop();
		}
	}

	void LoDSystem::UpdateAgainstPlayer()
	{
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
		ComponentPool& loDPool = scene.GetComponentPool<LoDComponent>();
		ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();

		EntityID playerID = scene.GetPlayerId();

		if (!scene.IsEntityIDValid(playerID))
			return;

		TransformComponent* pTransform = (TransformComponent*)transformPool.Get(GetEntityIndex(playerID));

		Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();

		float playerDist = (camera.GetPosition() - pTransform->myPosition).LengthSqr();

		struct EventData
		{
			EntityID entity;
			LODLEVEL lodLevel;
		};

		std::queue<EventData> events;

		LoDBox box;

		CU::Vec3f modelSize;
		TransformComponent* eTransform = nullptr;
		LoDComponent* lodComp = nullptr;
		ModelComponent* modelComp = nullptr;
		Model* model = nullptr;

		LoDLine cameraPlayerLine;

		CU::Vec4f temp;
		CU::Vec3f cameraPos = camera.GetPosition();

		CU::Matrix4x4f inverseMatrix;

		bool isIntersecting;

		for (auto& entityID : myEntities[0])
		{
			EntityIndex index = GetEntityIndex(entityID);

			eTransform = (TransformComponent*)transformPool.Get(index);
			lodComp = (LoDComponent*)loDPool.Get(index);
			modelComp = (ModelComponent*)modelPool.Get(index);

			model = Context::Get()->myModelManager->GetModel(modelComp->myModelId);

			inverseMatrix = CU::Matrix4x4f::Inverse(eTransform->myCachedTransform);

			temp = CU::Vec4f(cameraPos.x, cameraPos.y, cameraPos.z, 1) * inverseMatrix;
			cameraPlayerLine.myFrom = CU::Vec3f(temp.x, temp.y, temp.z);

			temp = CU::Vec4f(pTransform->myPosition.x, pTransform->myPosition.y, pTransform->myPosition.z, 1) * inverseMatrix;
			cameraPlayerLine.myTo = CU::Vec3f(temp.x, temp.y, temp.z);

			box.myMin = model->myAABB.myMin;
			box.myMax = model->myAABB.myMax;

			isIntersecting = IsIntersecting(cameraPlayerLine, box);

			if (lodComp->myCurrentLoDLevel == 0 && isIntersecting)
			{
				lodComp->myCurrentLoDLevel = 1;
				events.push({ entityID, 1 });

				Insert(myFadingEntites, entityID, -1);
				CustomShaderComponent* cSC = scene.AssignComponent<CustomShaderComponent>(entityID);
				cSC->myElaspedTime = 1;
				
				cSC->myShader = Shaders::eLoDFade;

				eTransform->myScale -= { 0.001,0.001f ,0.001f };
				scene.ActivateEntity(lodComp->myLoDEntities[1]);

				//scene.DeactivateEntity(lodComp->myLoDEntities[0]);
				//scene.ActivateEntity(lodComp->myLoDEntities[1]);

				continue;
			}

			if (lodComp->myCurrentLoDLevel == 1 && !isIntersecting)
			{
				lodComp->myCurrentLoDLevel = 0;
				events.push({ entityID, 0 });

				Insert(myFadingEntites, entityID, 1);
				CustomShaderComponent* cSC = scene.AssignComponent<CustomShaderComponent>(entityID);
				cSC->myElaspedTime = 0;

				cSC->myShader = Shaders::eLoDFade;

				eTransform->myScale -= { 0.001, 0.001f, 0.001f };
				scene.ActivateEntity(lodComp->myLoDEntities[0]);

				//scene.DeactivateEntity(lodComp->myLoDEntities[1]);
				//scene.ActivateEntity(lodComp->myLoDEntities[0]);
			}

			//float entityDist = (camera.GetPosition() - eTransform->myPosition).LengthSqr();

			//if (entityDist < playerDist && lodComp->myCurrentLoDLevel == 0)
			//{
			//	lodComp->myCurrentLoDLevel = 1;
			//	events.push({ entityID, 1 });

			//	scene.DeactivateEntity(lodComp->myLoDEntities[0]);
			//	scene.ActivateEntity(lodComp->myLoDEntities[1]);
			//	 
			//	

			//	continue;
			//}

			//if (entityDist > playerDist && lodComp->myCurrentLoDLevel == 1)
			//{
			//	lodComp->myCurrentLoDLevel = 0;
			//	events.push({ entityID, 0 });

			//	scene.DeactivateEntity(lodComp->myLoDEntities[1]);
			//	scene.ActivateEntity(lodComp->myLoDEntities[0]);
			//	

			//}

		}


		while (!events.empty())
		{
			EventData& data = events.front();
			for (auto& [identifier, callback] : myCallBacks)
			{
				callback(data.entity, data.lodLevel);
			}

			events.pop();
		}

		UpdateFades();

	}

	void LoDSystem::UpdateFades()
	{
		float delta = Context::Get()->myTimeHandler->GetDeltaTime();

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		ComponentPool& cPool = scene.GetComponentPool<CustomShaderComponent>();

		CustomShaderComponent* cSC = nullptr;
		EntityIndex index;

		for (auto& fade : myFadingEntites)
		{
			index = GetEntityIndex(fade.myID);
			cSC = (CustomShaderComponent*)cPool.Get(index);

			cSC->myElaspedTime += delta * fade.myProperty;
		}

		ComponentPool& loDPool = scene.GetComponentPool<LoDComponent>();
		LoDComponent* lodComp = nullptr;

		ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
		TransformComponent* tComponent = nullptr;

		for (int i = myFadingEntites.size() - 1; i >= 0; i--)
		{
			const EntityFades& fade = myFadingEntites[i];

			index = GetEntityIndex(fade.myID);
			cSC = (CustomShaderComponent*)cPool.Get(index);
			lodComp = (LoDComponent*)loDPool.Get(index);
			tComponent = (TransformComponent*)transformPool.Get(index);

			if (cSC == nullptr)
			{
				myFadingEntites.erase(myFadingEntites.begin() + i);
				continue;
			}

			if (cSC->myElaspedTime <= 0)
			{

				scene.RemoveComponent<CustomShaderComponent>(fade.myID);

				scene.DeactivateEntity(lodComp->myLoDEntities[0]);

				tComponent->myScale += { 0.001, 0.001f, 0.001f };

				myFadingEntites.erase(myFadingEntites.begin() + i);
				continue;
			}

			if (cSC->myElaspedTime >= 1)
			{

				scene.RemoveComponent<CustomShaderComponent>(fade.myID);

				scene.DeactivateEntity(lodComp->myLoDEntities[1]);
				//scene.ActivateEntity(lodComp->myLoDEntities[0]);

				tComponent->myScale += { 0.001, 0.001f, 0.001f };

				myFadingEntites.erase(myFadingEntites.begin() + i);
				continue;
			}

		}

	}

	void LoDSystem::Remove(std::vector<EntityFades>& someFades, const EntityID& aId)
	{
		size_t i = 0;
		for (auto& entityFade : someFades)
		{
			if (entityFade.myID == aId)
			{
				someFades.erase(someFades.begin() + i);
				break;
			}

			i++;
		}
	}

	void LoDSystem::Insert(std::vector<EntityFades>& someFades, const EntityID& aId, const int aProperty)
	{
		bool contains = false;

		for (auto& entityFade : someFades)
		{
			if (entityFade.myID == aId)
			{
				contains = true;
				entityFade.myProperty = aProperty;
				break;
			}
		}

		if (!contains)
		{
			EntityFades fade;
			fade.myID = aId;
			fade.myProperty = aProperty;
			someFades.push_back(fade);
		}
	}

}