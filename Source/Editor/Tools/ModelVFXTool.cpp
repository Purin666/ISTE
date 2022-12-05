#include "ModelVFXTool.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Helper/JsonIO.h"
#include "ISTE/Helper/JsonDefines.h" 

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/CU/InputHandler.h"

#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXDefines.h"

#include "VFXToolFunctions.h"

void ISTE::ModelVFXTool::Init(BaseEditor*)
{
	myCtx = Context::Get();
	if (!myCtx->myModelVFXHandler->mySystem->myDatas.empty())
	{
		mySelectedData = myCtx->myModelVFXHandler->mySystem->myDatas.begin()->first;
	}
	myToolName = "Model VFX Tool";
}

void ISTE::ModelVFXTool::Draw()
{
	if (!myActive)
		return;

	ModelVFXHandler* handler = myCtx->myModelVFXHandler;
	auto& datas = handler->mySystem->myDatas;

	ImGui::PushID(&datas);

	ImGui::Begin(myToolName.data(), &myActive);

	static std::string newName;

	if (ImGui::Button("Reload from JSON"))
	{
		LoadJson("../Assets/VFX/Data/ModelVFXTypes.json", datas);
		handler->LoadModelsAndTextures();
	}

	// Save, New, Remove
	ImGui::SameLine();
	if (ImGui::Button("Save All"))
		SaveJson("../Assets/VFX/Data/ModelVFXTypes.json", datas);

	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		newName = "Default_" + std::to_string(datas.size());
		datas[newName] = ModelVFXData();
		mySelectedData = newName;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		datas.erase(mySelectedData);
		mySelectedData = datas.begin()->first;
	}
	ImGui::SameLine();
	if (!datas.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newName = mySelectedData;
			const size_t selectedSize = mySelectedData.size();
			size_t suffix = 0;
			for (auto& e : datas)
			{
				if (e.first.substr(0, selectedSize) == mySelectedData)
					suffix++;
			}
			newName = newName + std::to_string(suffix);
			datas[newName] = datas[mySelectedData];
			mySelectedData = newName;
		}
	}

	// Select, Rename
	if (ImGui::BeginCombo("Selected", mySelectedData.data()))
	{
		for (auto& data : datas)
		{
			const bool selected = (mySelectedData == data.first);

			if (ImGui::Selectable(data.first.data(), selected))
				mySelectedData = data.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::InputText("Rename", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (datas.count(newName) == 0)
		{
			datas[newName] = datas[mySelectedData];
			datas.erase(mySelectedData);
			mySelectedData = newName;
		}
	}
	// Spawn, Deactivate, Kill
	if (ImGui::Button("Spawn at origin"))
		myEmitterId = handler->Spawn(mySelectedData, CU::Vec3f(0, 3, 0));

	ImGui::SameLine();
	if (ImGui::Button("Kill"))
		handler->Kill(myEmitterId);

	ImGui::SameLine();
	if (ImGui::Button("Kill All"))
		handler->KillAll();

	const std::string amount = std::to_string(myCtx->myModelVFXHandler->mySystem->myVFX.size());
	ImGui::SameLine();
	ImGui::Text(amount.data());

	newName = "";

	ImGui::Separator();
	DrawModelVFXEditor(datas[mySelectedData]);

	ImGui::End();

	ImGui::PopID();
}