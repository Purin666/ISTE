#include "PPEffectsTool.h"
#include "imgui/imgui.h"
#include "ISTE/Math/Vec.h"
#include "ISTE/CU/Database.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Context.h"

void ISTE::PPEffectsTool::Init(BaseEditor*)
{
	myToolName = "Post Processing Effects";
}

void ISTE::PPEffectsTool::Draw()
{
	if (!myActive)
	{
		return;
	}
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("PostProcess Effects", &myActive, flags);
	
	PPSliders();

	ImGui::End();
	
}

void ISTE::PPEffectsTool::PPSliders()
{
	CU::Database<true>& db = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetDatabase();

	CU::Vec3f& dbContrast = db.Get<CU::Vec3f>("Contrast");
	CU::Vec3f& dbTint = db.Get<CU::Vec3f>("Tint");
	CU::Vec3f& dbBlackpoint = db.Get<CU::Vec3f>("Blackpoint");

	float& bloomBlending = db.Get<float>("BloomBlending");
	float& saturation = db.Get<float>("Saturation");
	float& exposure = db.Get<float>("Exposure");
	float contrast[3] = { dbContrast.x, dbContrast.y, dbContrast.z };
	float tint[3] = { dbTint.x, dbTint.y, dbTint.z };
	float blackpoint[3] = { dbBlackpoint.x, dbBlackpoint.y, dbBlackpoint.z };

	ImGui::Text("Current values");
	ImGui::SameLine();

	ImGui::SliderFloat("Bloom", &bloomBlending, 0.f, 1.f);
	ImGui::SliderFloat("Saturation", &saturation, 0.f, 2.f);
	ImGui::SliderFloat("Exposure", &exposure, -5.f, 5.f);

	if (ImGui::SliderFloat3("Contrast", contrast, 0.f, 2.f))
		dbContrast = CU::Vec3f(contrast[0], contrast[1], contrast[2]);

	if (ImGui::ColorEdit3("Tint", tint))
		dbTint = CU::Vec3f(tint[0], tint[1], tint[2]);

	if (ImGui::ColorEdit3("Blackpoint", blackpoint))
		dbBlackpoint = CU::Vec3f(blackpoint[0], blackpoint[1], blackpoint[2]);
}

void ISTE::PPEffectsTool::PPSelect()
{
}

void ISTE::PPEffectsTool::PPSet()
{
}

void ISTE::PPEffectsTool::PPAddNew()
{
}

void ISTE::PPEffectsTool::PPRename()
{
}

void ISTE::PPEffectsTool::PPRemove()
{
}

void ISTE::PPEffectsTool::PPBlending()
{
}
