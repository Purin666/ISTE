#include "CullingSystem.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/GraphicsEngine.h"

#include "TransformComponent.h"
#include "ModelComponent.h"

#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/CU/RayFromScreen.h"
#include "ISTE/WindowsWindow.h"

#include "ISTE/Graphics/DebugDrawer.h"

#include "ISTE/Context.h"
#include "ISTE/Events/EventHandler.h"

struct Box
{
	CU::Vec3f mySize;
	CU::Vec3f myPos;
};

bool IsIntersecting(Box& aBox, Cell& aOtherBox)
{
	CU::Vec3f halfSize = aBox.mySize / 2.f;
	CU::Vec3f otherHalfSize = aOtherBox.mySize / 2.f;

	if ((aBox.myPos.x - halfSize.x) > (aOtherBox.myPosition.x + otherHalfSize.x)) return false;
	if ((aBox.myPos.x + halfSize.x) < (aOtherBox.myPosition.x - otherHalfSize.x)) return false;
	if ((aBox.myPos.z - halfSize.z) > (aOtherBox.myPosition.z + otherHalfSize.z)) return false;
	if ((aBox.myPos.z + halfSize.z) < (aOtherBox.myPosition.z - otherHalfSize.z)) return false;

	return true;
}

bool FrustumCellCheck(Cell& aCell, ISTE::Frustum& aFrustum)
{
	////ugly for now
	CU::Vec4f pos = CU::Vec4f(aCell.myPosition.x, aCell.myPosition.y, aCell.myPosition.z, 1);
	float xHalf = aCell.mySize.x / 2.f;
	float yHalf = aCell.mySize.y / 2.f;
	float zHalf = aCell.mySize.z / 2.f;

	CU::Vec3f nBL = CU::Vec3f(pos.x - xHalf, pos.y - yHalf, pos.z - zHalf);
	//CU::Vec3f nBR = CU::Vec3f(pos.x + xHalf, pos.y - yHalf, pos.z - zHalf);
	//CU::Vec3f nTL = CU::Vec3f(pos.x - xHalf, pos.y + yHalf, pos.z - zHalf);
	//CU::Vec3f nTR = CU::Vec3f(pos.x + xHalf, pos.y + yHalf, pos.z - zHalf);

	//CU::Vec3f fBL = CU::Vec3f(pos.x - xHalf, pos.y - yHalf, pos.z + zHalf);
	//CU::Vec3f fBR = CU::Vec3f(pos.x + xHalf, pos.y - yHalf, pos.z + zHalf);
	//CU::Vec3f fTL = CU::Vec3f(pos.x - xHalf, pos.y + yHalf, pos.z + zHalf);
	CU::Vec3f fTR = CU::Vec3f(pos.x + xHalf, pos.y + yHalf, pos.z + zHalf);

	int inside = 1;
	int checkValue = 1 << aFrustum.myPlanesAmount;

	//we could maybe just check the minimum and maximum points considering that the cells should be axis alligned

	for (int i = 0; i < aFrustum.myPlanesAmount; i++)
	{
		float dist = aFrustum.myPlanes[i].Distance(nBL);
		if (dist < 0) { inside <<= 1; continue; }

		//dist = aFrustum.myPlanes[i].Distance(nBR);
		//if (dist < 0) { inside <<= 1; continue; }

		//dist = aFrustum.myPlanes[i].Distance(nTL);
		//if (dist < 0) { inside <<= 1; continue; }

		//dist = aFrustum.myPlanes[i].Distance(nTR);
		//if (dist < 0) { inside <<= 1; continue; }

		//dist = aFrustum.myPlanes[i].Distance(fBL);
		//if (dist < 0) { inside <<= 1; continue; }

		//dist = aFrustum.myPlanes[i].Distance(fBR);
		//if (dist < 0) { inside <<= 1; continue; }

		//dist = aFrustum.myPlanes[i].Distance(fTL);
		//if (dist < 0) { inside <<= 1; continue; }

		dist = aFrustum.myPlanes[i].Distance(fTR);
		if (dist < 0) { inside <<= 1; continue; }

	}

	if (inside == checkValue) return true;

	return false;

}

namespace ISTE
{
	CullingSystem::CullingSystem()
	{
		Context::Get()->myEventHandler->RegisterCallback(ISTE::EventType::EntityMoved, "CullingSystem", [this](EntityID aId) { Update(aId); });
		Context::Get()->myEventHandler->RegisterCallback(ISTE::EventType::EntityDestroyed, "CullingSystem", [this](EntityID aId) { EntityDestroyed(aId); });
	}
	CullingSystem::~CullingSystem()
	{
	}
	void CullingSystem::Init()
	{
		myTransformComponentID = Context::Get()->mySceneHandler->GetId<TransformComponent>();
		myModelComponentID = Context::Get()->mySceneHandler->GetId<ModelComponent>();

	}
	void CullingSystem::SceneLoaded()
	{

		int splits = UwUSplits * UwUSplits;

		for (int i = 0; i < splits; i++)
		{
			myCells[i].myEntities.clear();
		}

#ifdef _CULLING_UPDATE_OPTIMIZATION_

		for (int i = 0; i < MAX_ENTITIES; i++)
			myIDTOIDMAPPING->clear();

#endif

		myLastrelevantCells.clear();

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		CU::Vec3f maxB = scene.GetDatabase().Get<CU::Vec3f>("MaxBounds");
		CU::Vec3f minB = scene.GetDatabase().Get<CU::Vec3f>("MinBounds");

		CU::Vec3f size = CU::Vec3f(abs(maxB.x - minB.x), abs(maxB.y - minB.y), abs(maxB.z - minB.z));

		CU::Vec3f cellSize = (size / (float)UwUSplits);
		CU::Vec3f halfSize = cellSize / 2.f;

		for (int x = 0; x < UwUSplits; x++)
		{
			for (int z = 0; z < UwUSplits; z++)
			{
				int cell = z + (x * UwUSplits);

				myCells[cell].myPosition = CU::Vec3f(minB.x + halfSize.x + (x * cellSize.x), minB.y + (size.y / 2.f), minB.z + halfSize.z + (z * cellSize.z));
				myCells[cell].mySize = cellSize;
				myCells[cell].mySize.y = size.y;
				myCells[cell].id = cell;

			}
		}

		ComponentPool& tPool = scene.GetComponentPool(myTransformComponentID);
		ComponentPool& mPool = scene.GetComponentPool(myModelComponentID);

		for (auto& id : myEntities[0])
		{
			TransformComponent* transform = (TransformComponent*)tPool.Get(GetEntityIndex(id));
			ModelComponent* model = (ModelComponent*)mPool.Get(GetEntityIndex(id));

			Model::AABBData data = Context::Get()->myModelManager->GetModel(model->myModelId)->myAABB;

			Box entBox;
			entBox.myPos = transform->myPosition;
			entBox.mySize = (data.myMax - data.myMin) * transform->myScale;

			for (int i = 0; i < splits; i++)
			{
				if (IsIntersecting(entBox, myCells[i]))
				{
					myCells[i].myEntities.insert(id);

#ifdef _CULLING_UPDATE_OPTIMIZATION_
					myIDTOIDMAPPING[GetEntityIndex(id)].push_back(myCells[i].id);
					//myEntityStates[GetEntityIndex(id)] = scene.IsEntityActive(id);
#endif
				}

			}

		}

		//DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
		//drawer.ClearStaticCommands();

		//for (int i = 0; i < splits; i++)
		//{
		//	LineCommand line;
		//	float r = ((float)myCells[i].myEntities.size() /8.f);
		//	line.myColor = CU::Vec3f(r, 0, 0);
		//	
		//	line.myFromPosition = myCells[i].myPosition - CU::Vec3f(halfSize.x, 0, halfSize.z);
		//	line.myToPosition = myCells[i].myPosition - CU::Vec3f(halfSize.x, 0, halfSize.z) + CU::Vec3f(cellSize.x, 0, 0);
		//	drawer.AddStaticLineCommand(line);
		//	line.myFromPosition = myCells[i].myPosition - CU::Vec3f(halfSize.x, 0, halfSize.z);
		//	line.myToPosition = myCells[i].myPosition - CU::Vec3f(halfSize.x, 0, halfSize.z) + CU::Vec3f(0, 0, cellSize.z);
		//	drawer.AddStaticLineCommand(line);
		//	line.myFromPosition = myCells[i].myPosition + CU::Vec3f(halfSize.x, 0, halfSize.z);
		//	line.myToPosition = myCells[i].myPosition + CU::Vec3f(halfSize.x, 0, halfSize.z) - CU::Vec3f(cellSize.x, 0, 0);
		//	drawer.AddStaticLineCommand(line);
		//	line.myFromPosition = myCells[i].myPosition + CU::Vec3f(halfSize.x, 0, halfSize.z);
		//	line.myToPosition = myCells[i].myPosition + CU::Vec3f(halfSize.x, 0, halfSize.z) - CU::Vec3f(0, 0, cellSize.z);
		//	drawer.AddStaticLineCommand(line);
		//}

	}
	void CullingSystem::Update(EntityID aId)
	{
		int splits = UwUSplits * UwUSplits;
		//this should maybe just add ids to a vector that is ran trough if needed, this would be a more effective way of getting the components in case of multiple moves in a frame

#ifndef _CULLING_UPDATE_OPTIMIZATION_

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		TransformComponent* transform = (TransformComponent*)scene.GetComponent(myTransformComponentID, aId);
		ModelComponent* model = (ModelComponent*)scene.GetComponent(myModelComponentID, aId);

		if (model == nullptr)
			return;

		Model::AABBData data = Context::Get()->myModelManager->GetModel(model->myModelId)->myAABB;

		Box entBox;

		entBox.myPos = transform->myCachedTransform.GetTranslationV3();
		entBox.mySize = (data.myMax - data.myMin) * transform->myCachedTransform.DecomposeScale();

		for (int i = 0; i < splits; i++)
		{
			if (IsIntersecting(entBox, myCells[i]))
			{
				myCells[i].myEntities.insert(aId);
			}
			else
			{
				myCells[i].myEntities.erase(aId);
			}

		}
#endif

#ifdef _CULLING_UPDATE_OPTIMIZATION_
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

		TransformComponent* transform = (TransformComponent*)scene.GetComponent(myTransformComponentID, aId);
		ModelComponent* model = (ModelComponent*)scene.GetComponent(myModelComponentID, aId);

		if (model == nullptr)
			return;

		Model::AABBData data = Context::Get()->myModelManager->GetModel(model->myModelId)->myAABB;

		Box entBox;

		entBox.myPos = transform->myCachedTransform.GetTranslationV3();
		entBox.mySize = (data.myMax - data.myMin) * transform->myCachedTransform.DecomposeScale();

		for (int i = 0; i < splits; i++)
		{
			bool contains = false;
			int pos = 0;

			for (auto& id : myIDTOIDMAPPING[GetEntityIndex(aId)])
			{
				if (id == myCells[i].id)
				{
					contains = true;
					break;
				}
				pos++;
			}

			bool isIntersectiong = IsIntersecting(entBox, myCells[i]);
			if (isIntersectiong && !contains)
			{
				myCells[i].myEntities.insert(aId);
				myIDTOIDMAPPING[GetEntityIndex(aId)].push_back(myCells[i].id);
			}
			else if (!isIntersectiong && contains)
			{
				myCells[i].myEntities.erase(aId);
				myIDTOIDMAPPING[GetEntityIndex(aId)].erase(myIDTOIDMAPPING[GetEntityIndex(aId)].begin() + pos);
			}

		}
#endif

	}

	void CullingSystem::EntityDestroyed(EntityID aId)
	{

#ifndef _CULLING_UPDATE_OPTIMIZATION_

		int splits = UwUSplits * UwUSplits;

		for (auto& cell : myCells)
		{
			for (auto& id : cell.myEntities)
			{
				if (id == aId)
				{
					cell.myEntities.erase(id);
					break;
				}
			}
		}

#endif // !_CULLING_UPDATE_OPTIMIZATION_


#ifdef _CULLING_UPDATE_OPTIMIZATION_

		for (auto& cellId : myIDTOIDMAPPING[GetEntityIndex(aId)])
		{
			myCells[cellId].myEntities.erase(aId);
		}

#endif

	}

	void CullingSystem::Cull()
	{
#ifdef CULLING

		myCheckDelayCount--;

		if (myCheckDelayCount >= 0)
			return;

		myCheckDelayCount = myCheckDelay;

		Camera& camera = Context::Get()->myGraphicsEngine->GetCamera();
		ISTE::Frustum frustum = camera.GetFrustum();

		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

#ifndef _CULLING_UPDATE_OPTIMIZATION_

		std::vector<Cell> relevantCells;

		for (int i = 0; i < UwUSplits * UwUSplits; i++)
		{
			if (FrustumCellCheck(myCells[i], frustum))
			{
				relevantCells.push_back(myCells[i]);
			}

		}

		//this whole thing is apperantly very expensive
		std::vector<Cell> myDeactivateCells;
		std::vector<Cell> myActivateCells;

		{

			for (auto& lastCell : myLastrelevantCells)
			{
				bool foundMatch = false;
				for (auto& nowCell : relevantCells)
				{
					if (lastCell.id == nowCell.id)
					{
						foundMatch = true;
						myActivateCells.push_back(nowCell);
						break;
					}
				}

				if (!foundMatch)
					myDeactivateCells.push_back(lastCell);
			}
		}

		for (auto& cell : myDeactivateCells)
		{
			for (auto& id : cell.myEntities)
			{
				scene.DeactivateEntity(id, true);
			}
		}

		for (auto& cell : myActivateCells)
		{
			for (auto& id : cell.myEntities)
			{
				scene.ActivateEntity(id, true);
			}
		}

		myLastrelevantCells = relevantCells;
#endif // !_CULLING_UPDATE_OPTIMIZATION_

#ifdef _CULLING_UPDATE_OPTIMIZATION_

		for (int i = 0; i < UwUSplits * UwUSplits; i++)
		{
			if (FrustumCellCheck(myCells[i], frustum))
			{
				myCells[i].myIsActiveState = true;
				continue;
			}

			myCells[i].myIsActiveState = false;

		}

		for (int i = 0; i < UwUSplits * UwUSplits; i++)
		{
			for (const EntityID entityID : myCells[i].myEntities)
			{
				bool wasActive = false;

				for (const int cellID : myIDTOIDMAPPING[GetEntityIndex(entityID)])
				{
					if (myCells[cellID].myIsActiveState)
					{
						wasActive = true;
						scene.ActivateEntity(entityID, true);
						break;
					}
				}

				if(!wasActive)
					scene.DeactivateEntity(entityID, true);
			}
		}


#endif // _CULLING_UPDATE_OPTIMIZATION_



#endif // !CULLING

	}
}