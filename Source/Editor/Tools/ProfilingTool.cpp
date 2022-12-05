#include "ProfilingTool.h"
#include "ISTE/Context.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Logger/Logger.h"
#include "ISTE/CU/MemoryTracker.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "imgui/imgui.h"
#include <string>

void ISTE::ProfilingTool::Init(BaseEditor*)
{
	myToolName = "Profiler";
}

void ISTE::ProfilingTool::Draw()
{
	if (!myActive)
	{
		return;
	}
	ImGui::Begin("Profiler", &myActive);
	ImGui::Separator();
	FPS();
	ImGui::Separator();
	DrawCalls();
	ImGui::Separator();
	Allocations();
	ImGui::Separator();
	Entities();
	ImGui::Separator();
	ImGui::End();
}

void ISTE::ProfilingTool::DrawCalls()
{ 
	std::string calls = "Draw Calls: ";
	calls = calls + std::to_string(Logger::GetDrawCalls()).c_str();
	ImGui::Text(calls.c_str());

	calls = "Batched Draw Calls: ";
	calls = calls + std::to_string(Logger::GetBatchedDrawCalls()).c_str();
	ImGui::Text(calls.c_str());
}

void ISTE::ProfilingTool::FPS()
{
	myFpsCount++;
	myElepsedTime += Context::Get()->myTimeHandler->GetDeltaTime();
	if (myElepsedTime >= 1)
	{
		myLastFps = myFpsCount;
		myElepsedTime = 0;
		myFpsCount = 0;
	}
	std::string fpsLogg = "FPS: " + std::to_string(myLastFps);
	ImGui::Text(fpsLogg.c_str());

}

void ISTE::ProfilingTool::Allocations()
{
	std::string allocationSize = "Allocated Bytes: " + std::to_string(CU::MemoryTracker::GetTotalAllocationCount() / 1000) + " kb";
	ImGui::Text(allocationSize.c_str());
	//allocationSize = "Allocated Objects: " + std::to_string(CU::MemoryTracker::GetTotalAllocatedOjbects());
	//ImGui::Text(allocationSize.c_str());
}

void ISTE::ProfilingTool::Entities()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	SceneHandler* sceneHandler = Context::Get()->mySceneHandler;

	std::string entities = "Entities: " + std::to_string(scene.myEntities.size() - scene.myFreeIndexes.size());
	ImGui::Text(entities.c_str());

	size_t activeEntities = 0;

	for (auto& ent : scene.myEntities)
	{
		if (!IsEntityIndexValid(ent.myId))
			continue;

		activeEntities += ent.myIsActive;
	}

	std::string activeEnts = "Active Entities: " + std::to_string(activeEntities);
	ImGui::Text(activeEnts.c_str());

	std::string pools = "Pools: " + std::to_string(scene.myComponentPools.size());
	ImGui::Text(pools.c_str());

	if (ImGui::TreeNode("Pool Data"))
	{
		for (auto& pool : scene.myComponentPools)
		{
			ImGui::Separator();
			std::string poolName = sceneHandler->myComponentNames[pool.first];

			ImGui::Text(poolName.c_str());
			ImGui::Text(("MappedComponents: " + std::to_string(pool.second->myMappedIndexes - pool.second->myFreeSpots.size())).c_str());
			ImGui::Text(("ComponentSize: " + std::to_string(pool.second->myComponentSize)).c_str());
			ImGui::Text(("ComponentCap: " + std::to_string(pool.second->myComponentAmount)).c_str());
		}


		ImGui::TreePop();
	}
}
