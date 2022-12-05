#include "Particle3DTool.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Helper/JsonIO.h"
#include "ISTE/Helper/JsonDefines.h" 

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/CU/InputHandler.h"

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleDefines.h"

#include "VFXToolFunctions.h"

void ISTE::Particle3DTool::Init(BaseEditor*)
{
	myCtx = Context::Get();

	auto& datas = myCtx->mySprite3DParticleHandler->mySystem->myEmitterDatas;
	if (!datas.empty())
		mySelectedData = datas.begin()->first;

	myToolName = "Particle 3D Tool";
}

void ISTE::Particle3DTool::Draw()
{
	if (!myActive)
		return;

	Sprite3DParticleHandler* handler = myCtx->mySprite3DParticleHandler;
	auto& emitterDatas = handler->mySystem->myEmitterDatas;

	ImGui::PushID(&emitterDatas);

	ImGui::Begin(myToolName.data(), &myActive);

	static std::string newName;

	if (ImGui::Button("Reload from JSON"))
	{
		LoadJson("../Assets/VFX/Data/Emitter3DTypes.json", emitterDatas);
		handler->LoadParticleTextures();
	}
	// Save, New, Remove
	ImGui::SameLine();
	if (ImGui::Button("Save All"))
		SaveJson("../Assets/VFX/Data/Emitter3DTypes.json", emitterDatas);
	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		const std::string newName = "Default_" + std::to_string(emitterDatas.size());
		emitterDatas[newName] = Emitter3DData();
		mySelectedData = newName;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		emitterDatas.erase(mySelectedData);
		mySelectedData = emitterDatas.begin()->first;
	}
	ImGui::SameLine();
	if (!emitterDatas.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newName = mySelectedData;
			const size_t selectedSize = mySelectedData.size();
			size_t suffix = 0;
			for (auto& e : emitterDatas)
			{
				if (e.first.substr(0, selectedSize) == mySelectedData)
					suffix++;
			}
			newName = newName + std::to_string(suffix);
			emitterDatas[newName] = emitterDatas[mySelectedData];
			mySelectedData = newName;
		}
	}
	// Select, Rename
	if (ImGui::BeginCombo("Selected", mySelectedData.data()))
	{
		for (auto& data : emitterDatas)
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
		if (emitterDatas.count(newName) == 0)
		{
			emitterDatas[newName] = emitterDatas[mySelectedData];
			emitterDatas.erase(mySelectedData);
			mySelectedData = newName;
		}
	}
	// Spawn, Deactivate, Kill
	if (ImGui::Button("Spawn at origin"))
	{
		myEmitterId = handler->SpawnEmitter(mySelectedData, CU::Vec3f(0, 3, 0));
	}
	//ImGui::SameLine();
	//if (ImGui::Button("Selected"))
	//	myEmitterId = handler->SpawnEmitter(mySelectedData, CU::Vec3f(0, 3, 0));
	ImGui::SameLine();
	if (ImGui::Button("Deactivate"))
		handler->DeactivateEmitter(myEmitterId);
	ImGui::SameLine();
	if (ImGui::Button("Kill"))
		handler->KillEmitter(myEmitterId);
	ImGui::SameLine();
	if (ImGui::Button("Kill All"))
		handler->KillAllEmitters();

	newName = "";

	ImGui::Separator();
	DrawEmitter3DEditor(emitterDatas[mySelectedData]);

	ImGui::End();

	ImGui::PopID();
}