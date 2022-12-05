#include "VFXTool.h"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ISTE/Helper/JsonIO.h"
#include "ISTE/Helper/JsonDefines.h" 

#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/CU/InputHandler.h"

#include "ISTE/VFX/VFXHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"

#include "VFXToolFunctions.h"

void ISTE::VFXTool::Init(BaseEditor* aBaseEditor)
{
	myToolName = "VFX Tool";
	myCtx = Context::Get();

	// selected packagedata
	{
		auto& datas = myCtx->myVFXHandler->mySystem->myDatas;
		if (!datas.empty())
		{
			mySelectedPackageData = datas.begin()->first;
			auto& data = datas[mySelectedPackageData];
			mySelectedModel = (!data.myModelVFXTypes.empty()) ? data.myModelVFXTypes.front() : "N/A";
			mySelectedEmitter = (!data.myEmitterTypes.empty()) ? data.myEmitterTypes.front() : "N/A";
		}
	}
	// selected modelData
	{
		auto& datas = myCtx->myModelVFXHandler->mySystem->myDatas;
		if (!datas.empty())
			mySelectedModelData = datas.begin()->first;
	}
	// selected emitter3dData
	{
		auto& datas = myCtx->mySprite3DParticleHandler->mySystem->myEmitterDatas;
		if (!datas.empty())
			mySelectedEmitter3DData = datas.begin()->first;
	}
	// selected emitter2dData
	{
		auto& datas = myCtx->mySprite2DParticleHandler->mySystem->myEmitterDatas;
		if (!datas.empty())
			mySelectedEmitter2DData = datas.begin()->first;
	}
}

void ISTE::VFXTool::Draw()
{
	if (!myActive)
		return;

	ImGui::Begin(myToolName.data(), &myActive);

	ImGui::Text("Select Tool");
	static int radioSelectedTool = 0;
	ImGui::RadioButton("Package",	&radioSelectedTool, 0); ImGui::SameLine();
	ImGui::RadioButton("Model",		&radioSelectedTool, 1); ImGui::SameLine();
	ImGui::RadioButton("Emitter3D", &radioSelectedTool, 2); ImGui::SameLine();
	ImGui::RadioButton("Emitter2D", &radioSelectedTool, 3);
	mySelectedTool = (SelectedToolVFX)radioSelectedTool;
	ImGui::Separator();

	switch (mySelectedTool)
	{
	case ISTE::SelectedToolVFX::Package:
		DrawPackageTool();
		break;
	case ISTE::SelectedToolVFX::Model:
		DrawModelTool();
		break;
	case ISTE::SelectedToolVFX::Emitter3D:
		DrawEmitter3DTool();
		break;
	case ISTE::SelectedToolVFX::Emitter2D:
		DrawEmitter2DTool();
		break;
	}

	ImGui::End();
}

void ISTE::VFXTool::DrawPackageTool()
{
	ImGui::Text("Package Tool");
	DrawTopConfig();
	DrawSelection();
	switch (mySelectedEdit)
	{
	case ISTE::SelectedEditVFX::Models:
		if (mySelectedModel != "N/A")
		{
			auto& data = myCtx->myModelVFXHandler->mySystem->myDatas[mySelectedModel];
			DrawModelVFXEditor(data);
		}
		break;
	case ISTE::SelectedEditVFX::Emitters3D:
		if (mySelectedEmitter != "N/A")
		{
			auto& data = myCtx->mySprite3DParticleHandler->mySystem->myEmitterDatas[mySelectedEmitter];
			DrawEmitter3DEditor(data);
		}
		break;
	}
}
void ISTE::VFXTool::DrawModelTool()
{
	ImGui::Text("Model Tool");

	ModelVFXHandler* handler = myCtx->myModelVFXHandler;
	auto& datas = handler->mySystem->myDatas;

	ImGui::PushID(&datas);

	static std::string newModelName;

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
		newModelName = "Default_" + std::to_string(datas.size());
		datas[newModelName] = ModelVFXData();
		mySelectedModelData = newModelName;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		datas.erase(mySelectedModelData);
		mySelectedModelData = datas.begin()->first;
	}
	ImGui::SameLine();
	if (!datas.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newModelName = mySelectedModelData;
			const size_t selectedSize = mySelectedModelData.size();
			size_t suffix = 0;
			for (auto& e : datas)
			{
				if (e.first.substr(0, selectedSize) == mySelectedModelData)
					suffix++;
			}
			newModelName = newModelName + std::to_string(suffix);
			datas[newModelName] = datas[mySelectedModelData];
			mySelectedModelData = newModelName;
		}
	}

	// Select, Rename
	if (ImGui::BeginCombo("Selected", mySelectedModelData.data()))
	{
		for (auto& data : datas)
		{
			const bool selected = (mySelectedModelData == data.first);

			if (ImGui::Selectable(data.first.data(), selected))
				mySelectedModelData = data.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::InputText("Rename", &newModelName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (datas.count(newModelName) == 0)
		{
			datas[newModelName] = datas[mySelectedModelData];
			datas.erase(mySelectedModelData);
			mySelectedModelData = newModelName;
		}
	}
	// Spawn, Deactivate, Kill
	if (ImGui::Button("Spawn at origin"))
		myModelId = handler->Spawn(mySelectedModelData, CU::Vec3f(0, 3, 0));

	ImGui::SameLine();
	if (ImGui::Button("Kill"))
		handler->Kill(myModelId);

	ImGui::SameLine();
	if (ImGui::Button("Kill All"))
		handler->KillAll();

	const std::string amount = std::to_string(myCtx->myModelVFXHandler->mySystem->myVFX.size());
	ImGui::SameLine();
	ImGui::Text(amount.data());

	newModelName = "";

	ImGui::Separator();
	DrawModelVFXEditor(datas[mySelectedModelData]);

	ImGui::PopID();
}
void ISTE::VFXTool::DrawEmitter3DTool()
{
	ImGui::Text("Emitter3D Tool");

	Sprite3DParticleHandler* handler = myCtx->mySprite3DParticleHandler;
	auto& emitterDatas = handler->mySystem->myEmitterDatas;

	ImGui::PushID(&emitterDatas);

	static std::string newEmitter3DName;

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
		mySelectedEmitter3DData = newName;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		emitterDatas.erase(mySelectedEmitter3DData);
		mySelectedEmitter3DData = emitterDatas.begin()->first;
	}
	ImGui::SameLine();
	if (!emitterDatas.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newEmitter3DName = mySelectedEmitter3DData;
			const size_t selectedSize = mySelectedEmitter3DData.size();
			size_t suffix = 0;
			for (auto& e : emitterDatas)
			{
				if (e.first.substr(0, selectedSize) == mySelectedEmitter3DData)
					suffix++;
			}
			newEmitter3DName = newEmitter3DName + std::to_string(suffix);
			emitterDatas[newEmitter3DName] = emitterDatas[mySelectedEmitter3DData];
			mySelectedEmitter3DData = newEmitter3DName;
		}
	}
	// Select, Rename
	if (ImGui::BeginCombo("Selected", mySelectedEmitter3DData.data()))
	{
		for (auto& data : emitterDatas)
		{
			const bool selected = (mySelectedEmitter3DData == data.first);

			if (ImGui::Selectable(data.first.data(), selected))
				mySelectedEmitter3DData = data.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::InputText("Rename", &newEmitter3DName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (emitterDatas.count(newEmitter3DName) == 0)
		{
			emitterDatas[newEmitter3DName] = emitterDatas[mySelectedEmitter3DData];
			emitterDatas.erase(mySelectedEmitter3DData);
			mySelectedEmitter3DData = newEmitter3DName;
		}
	}
	// Spawn, Deactivate, Kill
	ImGui::PushButtonRepeat(true);
	if (ImGui::Button("Spawn at origin"))
		myEmitter3DIds.push_back(handler->SpawnEmitter(mySelectedEmitter3DData, CU::Vec3f(0, 3, 0)));
	ImGui::PopButtonRepeat();
	ImGui::SameLine();
	if (ImGui::Button("Deactivate"))
		if (!myEmitter3DIds.empty())
		{
			handler->DeactivateEmitter(myEmitter3DIds.back());
			myEmitter3DIds.pop_back();
		}
	ImGui::SameLine();
	if (ImGui::Button("Kill"))
		if (!myEmitter3DIds.empty())
		{
			handler->KillEmitter(myEmitter3DIds.back());
			myEmitter3DIds.pop_back();
		}
	ImGui::SameLine();
	if (ImGui::Button("Deact. All"))
		for (const int id : myEmitter3DIds)
			handler->DeactivateEmitter(id);
	ImGui::SameLine();
	if (ImGui::Button("Kill All"))
		for (const int id : myEmitter3DIds)
			handler->KillEmitter(id);

	newEmitter3DName = "";

	ImGui::Separator();
	DrawEmitter3DEditor(emitterDatas[mySelectedEmitter3DData]);

	ImGui::PopID();
}
void ISTE::VFXTool::DrawEmitter2DTool()
{
	ImGui::Text("Emitter2D Tool");

	Sprite2DParticleHandler* handler = myCtx->mySprite2DParticleHandler;
	auto& emitterDatas = handler->mySystem->myEmitterDatas;

	ImGui::PushID(&emitterDatas);

	static std::string newEmitter2DName;

	if (ImGui::Button("Reload from JSON"))
	{
		LoadJson("../Assets/VFX/Data/Emitter2DTypes.json", emitterDatas);
		handler->LoadParticleTextures();
	}
	// Save, New, Remove
	ImGui::SameLine();
	if (ImGui::Button("Save All"))
		SaveJson("../Assets/VFX/Data/Emitter2DTypes.json", emitterDatas);
	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		const std::string newName = "Default_" + std::to_string(emitterDatas.size());
		emitterDatas[newName] = Emitter2DData();
		mySelectedEmitter2DData = newName;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		emitterDatas.erase(mySelectedEmitter2DData);
		mySelectedEmitter2DData = emitterDatas.begin()->first;
	}
	ImGui::SameLine();
	if (!emitterDatas.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newEmitter2DName = mySelectedEmitter2DData;
			const size_t selectedSize = mySelectedEmitter2DData.size();
			size_t suffix = 0;
			for (auto& e : emitterDatas)
			{
				if (e.first.substr(0, selectedSize) == mySelectedEmitter2DData)
					suffix++;
			}
			newEmitter2DName = newEmitter2DName + std::to_string(suffix);
			emitterDatas[newEmitter2DName] = emitterDatas[mySelectedEmitter2DData];
			mySelectedEmitter2DData = newEmitter2DName;
		}
	}
	// Select, Rename
	if (ImGui::BeginCombo("Selected Emitter", mySelectedEmitter2DData.data()))
	{
		for (auto& data : emitterDatas)
		{
			const bool selected = (mySelectedEmitter2DData == data.first);

			if (ImGui::Selectable(data.first.data(), selected))
				mySelectedEmitter2DData = data.first;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::InputText("Rename", &newEmitter2DName, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (emitterDatas.count(newEmitter2DName) == 0)
		{
			emitterDatas[newEmitter2DName] = emitterDatas[mySelectedEmitter2DData];
			emitterDatas.erase(mySelectedEmitter2DData);
			mySelectedEmitter2DData = newEmitter2DName;
		}
	}
	// Spawn, Deactivate, Kill
	if (ImGui::Button("Spawn at origin"))
		myEmitter2DId = handler->SpawnEmitter(mySelectedEmitter2DData, CU::Vec2f(0, 0));
	//ImGui::SameLine();
	//if (ImGui::Button("Selected"))
	//	myEmitter2DId = handler->SpawnEmitter(mySelectedEmitter2DData, CU::Vec3f(0, 3, 0));
	ImGui::SameLine();
	if (ImGui::Button("Deactivate"))
		handler->DeactivateEmitter(myEmitter2DId);
	ImGui::SameLine();
	if (ImGui::Button("Kill"))
		handler->KillEmitter(myEmitter2DId);
	ImGui::SameLine();
	if (ImGui::Button("Kill All"))
		handler->KillAllEmitters();

	newEmitter2DName = "";

	ImGui::Separator();
	DrawEmitter2DEditor(emitterDatas[mySelectedEmitter2DData]);

	ImGui::PopID();
}

void ISTE::VFXTool::DrawTopConfig()
{
	VFXHandler* handler = myCtx->myVFXHandler;
	auto& datas = handler->mySystem->myDatas;

	ImGui::PushID(&datas);

	if (ImGui::Button("Reload from JSON"))
	{
		LoadJson("../Assets/VFX/Data/VFXData.json", datas);
		handler->LoadModelsAndTextures();
	}

	static std::string newName;

	// Save, New, Remove, Duplicate
	ImGui::SameLine();
	if (ImGui::Button("Save All"))
	{
		SaveJson("../Assets/VFX/Data/VFXData.json", datas);
		SaveJson("../Assets/VFX/Data/ModelVFXTypes.json", myCtx->myModelVFXHandler->mySystem->myDatas);
		SaveJson("../Assets/VFX/Data/Emitter3DTypes.json", myCtx->mySprite3DParticleHandler->mySystem->myEmitterDatas);
	}

	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		newName = "Default_" + std::to_string(datas.size());
		datas[newName] = VFXData();
		mySelectedPackageData = newName;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		datas.erase(mySelectedPackageData);
		mySelectedPackageData = datas.begin()->first;
	}
	ImGui::SameLine();
	if (!datas.empty())
	{
		if (ImGui::Button("Duplicate"))
		{
			newName = mySelectedPackageData;
			const size_t selectedSize = mySelectedPackageData.size();
			size_t suffix = 0;
			for (auto& e : datas)
			{
				if (e.first.substr(0, selectedSize) == mySelectedPackageData)
					suffix++;
			}
			newName = newName + std::to_string(suffix);
			datas[newName] = datas[mySelectedPackageData];
			mySelectedPackageData = newName;
		}
	}

	// Select, Rename
	if (ImGui::BeginCombo("Selected VFX", mySelectedPackageData.data()))
	{
		for (auto& [name, data] : datas)
		{
			const bool selected = (mySelectedPackageData == name);

			if (ImGui::Selectable(name.data(), selected))
			{
				mySelectedPackageData = name;

				mySelectedModel = (!data.myModelVFXTypes.empty()) ? data.myModelVFXTypes.front() : "N/A";
				mySelectedEmitter = (!data.myEmitterTypes.empty()) ? data.myEmitterTypes.front() : "N/A";
			}

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
			datas[newName] = datas[mySelectedPackageData];
			datas.erase(mySelectedPackageData);
			mySelectedPackageData = newName;
		}
	}
	// Spawn, Deactivate, Kill
	if (ImGui::Button("Spawn at origin"))
	{
		handler->KillVFX(myVFXId);
		myVFXId = handler->SpawnVFX(mySelectedPackageData, CU::Vec3f(0, 0, 0));
	}
	ImGui::SameLine();
	if (ImGui::Button("Spawn at Y.3"))
	{
		handler->KillVFX(myVFXId);
		myVFXId = handler->SpawnVFX(mySelectedPackageData, CU::Vec3f(0, 3, 0));
	}

	ImGui::SameLine();
	if (ImGui::Button("Kill"))
		handler->KillVFX(myVFXId);

	ImGui::SameLine();
	if (ImGui::Button("Kill All"))
		handler->KillAllVFX();

	const std::string amount = std::to_string(handler->mySystem->myPackages.size());
	ImGui::SameLine();
	ImGui::Text(amount.data());

	newName = "";

	ImGui::Separator();
	ImGui::PopID();
}

void ISTE::VFXTool::DrawSelection()
{
	VFXHandler* handler = myCtx->myVFXHandler;
	auto& datas = handler->mySystem->myDatas;

	if (!datas.empty())
	{
		auto& data = datas[mySelectedPackageData];

		static std::string newName;
		switch (mySelectedEdit)
		{
		case ISTE::SelectedEditVFX::Models:

			DrawControl(data, data.myModelVFXTypes, mySelectedModel, newName);
			break;
		case ISTE::SelectedEditVFX::Emitters3D:
			DrawControl(data, data.myEmitterTypes, mySelectedEmitter, newName);
			break;
		default:
			DrawControl(data, data.myModelVFXTypes, mySelectedModel, newName);
			break;
		}
	}
}

void ISTE::VFXTool::DrawControl(VFXData& data, std::vector<std::string>& names, std::string& selected, std::string& newNameStatic)
{
	ImGui::PushID(&data);

	ImGui::Text("Generic Data");
	ImGui::SameLine();
	ImGui::Checkbox("Loop", &data.myShouldLoop);
	ImGui::DragFloat2("Lifetime", (float*)&data.myLifetime, 0.01f, 0.f);
	ImGui::DragFloat2("Delay", (float*)&data.myDelay, 0.01f, 0.f);
	ImGui::DragFloat2("FirstDelay", (float*)&data.myFirstDelay, 0.01f, 0.f);
	ImGui::Separator();

	static int e = 0;
	ImGui::RadioButton("Model", &e, 0);		ImGui::SameLine();
	ImGui::RadioButton("Emitter3D", &e, 1);
	mySelectedEdit = (SelectedEditVFX)e;

	auto& modelDatas = myCtx->myModelVFXHandler->mySystem->myDatas;
	auto& emitterDatas = myCtx->mySprite3DParticleHandler->mySystem->myEmitterDatas;
	switch (mySelectedEdit)
	{
	case ISTE::SelectedEditVFX::Models:
		ImGui::Text("Add-Combo");
		if (ImGui::BeginCombo("Add Model", NULL))
		{
			for (auto& [name, data] : modelDatas)
			{
				if (ImGui::Selectable(name.data()))
				{
					names.push_back(name);
					selected = name;
				}
			}
			ImGui::EndCombo();
		}
		break;
	case ISTE::SelectedEditVFX::Emitters3D:
		ImGui::Text("Add-Combo");
		if (ImGui::BeginCombo("Add Emitter", NULL))
		{
			for (auto& [name, data] : emitterDatas)
			{
				if (ImGui::Selectable(name.data()))
				{
					names.push_back(name);
					selected = name;
				}
			}
			ImGui::EndCombo();
		}
		break;
	default:
		ImGui::PopID();
		return;
	}

	if (!names.empty())
	{
		if (ImGui::Button("Remove"))
		{
			auto& itr = std::find(names.begin(), names.end(), selected);
			if (itr != names.end())
				names.erase(itr);
			if (names.empty())
				selected = "N/A";
			else
				selected = names.front();
		}
		ImGui::SameLine();
		if (ImGui::Button("Duplicate"))
		{
			names.push_back(selected);
		}
		if (ImGui::BeginCombo("Selected", selected.data()))
		{
			for (std::string& name : names)
			{
				const bool flag = (selected == name);

				if (ImGui::Selectable(name.data(), flag))
					selected = name;
				if (flag)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
	}
	else
		selected = "N/A";

	ImGui::PopID();
}