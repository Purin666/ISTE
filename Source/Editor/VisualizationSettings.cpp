#include "VisualizationSettings.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/DebugDrawer.h"

#include "ISTE/Physics/ComponentsAndSystems/TriggerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimationDrawerSystem.h"

#include "EditorContext.h"
#include "Gizmos.h"

#include "imgui/imgui.h"

void ISTE::VisualizationSettings::StateCheck()
{
	if (ImGui::MenuItem("DisableAll"))
	{
		myCoverageFlag = 0;
	}

	VisualizationStateFlag fullFlag = (VisualizationStateFlag)VisualizationFlagValues::eFull;
	if (ImGui::MenuItem("EnableAll"))
	{
		myCoverageFlag = fullFlag;
	}


	bool visual = ((VisualizationStateFlag)VisualizationFlagValues::eNavMesh & myCoverageFlag);
	ImGui::MenuItem("NavMesh", nullptr, &visual);
	(visual ? myCoverageFlag |= (VisualizationStateFlag)VisualizationFlagValues::eNavMesh : myCoverageFlag &= (fullFlag ^ (VisualizationStateFlag)VisualizationFlagValues::eNavMesh));

	visual = ((VisualizationStateFlag)VisualizationFlagValues::eTriggers & myCoverageFlag);
	ImGui::MenuItem("Triggers", nullptr, &visual);
	(visual ? myCoverageFlag |= (VisualizationStateFlag)VisualizationFlagValues::eTriggers : myCoverageFlag &= (fullFlag ^ (VisualizationStateFlag)VisualizationFlagValues::eTriggers));

	visual = ((VisualizationStateFlag)VisualizationFlagValues::eSkeleton & myCoverageFlag);
	ImGui::MenuItem("Skeletons", nullptr, &visual);
	(visual ? myCoverageFlag |= (VisualizationStateFlag)VisualizationFlagValues::eSkeleton : myCoverageFlag &= (fullFlag ^ (VisualizationStateFlag)VisualizationFlagValues::eSkeleton));

}

void ISTE::VisualizationSettings::DrawVisuals()
{
	SystemManager* sm = Context::Get()->mySystemManager;

	if (myCoverageFlag & (VisualizationStateFlag)VisualizationFlagValues::eNavMesh)
		DrawNavMesh();

	if (myCoverageFlag & (VisualizationStateFlag)VisualizationFlagValues::eTriggers)
	{
		sm->GetSystem<TriggerSystem>()->DrawBoxes();
		sm->GetSystem<TriggerSystem>()->DrawSpheres();
	}

	if (myCoverageFlag & (VisualizationStateFlag)VisualizationFlagValues::eSkeleton) 
		DrawSkeleton(); 

}


void ISTE::VisualizationSettings::DrawNavMesh()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	NavMesh::NavMesh& navMesh = scene.GetNavMesh();
	DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
	for (auto& tri : navMesh.triangles)
	{
		for (size_t i = 1; i <= 3; ++i)
		{
			LineCommand line;
			line.myColor = CU::Vec3f(0, 1, 0);
			line.myFromPosition = *tri.vertices[i - 1];
			size_t iTo = i % 3;
			line.myToPosition = *tri.vertices[iTo];
			drawer.AddDynamicLineCommand(line);
		}

		for (int& conIndex : tri.connections)
		{
			LineCommand line;
			line.myColor = CU::Vec3f(1, 1, 0);
			line.myFromPosition = tri.Center();
			line.myToPosition = navMesh.triangles[conIndex].Center();
			drawer.AddDynamicLineCommand(line);
		}
	}
}

void ISTE::VisualizationSettings::DrawSkeleton()
{
	Context::Get()->mySystemManager->GetSystem<AnimationDrawerSystem>()->DrawVisualization();
}
