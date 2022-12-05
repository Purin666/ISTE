#include "FogTool.h"
#include "imgui/imgui.h"
#include "ISTE/CU/Database.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

void ISTE::FogTool::Init(BaseEditor*)
{
	myToolName = "Fog Tool";
}

void ISTE::FogTool::Draw()
{

	if (!myActive)
	{
		return;
	}
	ImGui::Begin("Fog Tool", &myActive);
	CU::Database<true>& db = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetDatabase();

	CU::Vec3f& dbFogColor = db.Get<CU::Vec3f>("FogColor");
	CU::Vec3f& dbFogHighlightColor = db.Get<CU::Vec3f>("FogHighlightColor");

	float& fogStartDist = db.Get<float>("FogStartDist");
	float& fogGlobalDensity = db.Get<float>("FogGlobalDensity");
	float& fogHeightFalloff = db.Get<float>("FogHeightFalloff");
	float fogColor[3] = { dbFogColor.x, dbFogColor.y, dbFogColor.z };
	float fogHighlightColor[3] = { dbFogHighlightColor.x, dbFogHighlightColor.y, dbFogHighlightColor.z };

	ImGui::Separator();
	ImGui::Spacing();
	if(ImGui::ColorEdit3("Fog color: ", fogColor))
	{
		dbFogColor = { fogColor[0],fogColor[1],fogColor[2] };
	}

	//sketch
	ImGui::DragFloat("Fog start distance: ", &fogStartDist, 0.1, 0, 1000, "%.3f", 1.01f);
	ImGui::DragFloat("Fog density: ", &fogGlobalDensity,0.01, 0, 1);
	ImGui::DragFloat("Fog height falloff: ", &fogHeightFalloff, 0.001f, 0, 1);

	if(ImGui::ColorEdit3("Fog highlight color: ", fogHighlightColor))
	{
		dbFogHighlightColor = { fogHighlightColor[0],fogHighlightColor[1],fogHighlightColor[2] };
	}


	ImGui::Spacing();
	ImGui::Separator();
	ImGui::End();
}
