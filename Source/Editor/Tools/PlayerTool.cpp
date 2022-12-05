#include "PlayerTool.h"

#include "imgui/imgui.h"

#include "ISTE/Context.h"

#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Events/EventHandler.h"

#include "ISTE/ECSB/PlayerBehaviour.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "Json/json.hpp"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include <fstream>

void ISTE::PlayerTool::Init(BaseEditor*)
{
	myToolName = "Player";
	myCtx = Context::Get();
}

void ISTE::PlayerTool::Draw()
{
	if (!myActive) return;



	ImGui::Begin("Player Tool", &myActive);
	if (ImGui::CollapsingHeader("Player Cheat Tool"))
	{
		DrawPlayerCheatTool(); 
	}
	if (ImGui::CollapsingHeader("Player Animation Lerp Tool"))
	{
		DrawAnimBlendTool();
	}
	ImGui::End();
}

void ISTE::PlayerTool::DrawPlayerCheatTool()
{

	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	EntityID playerId = scene.GetPlayerId();
	PlayerBehaviour* player = scene.GetComponent<PlayerBehaviour>(playerId);

	ImGui::Text("Misc:");
	if (ImGui::Button("Set OP (very much hp/mana")) { player->myHealth = 999'999.f;				player->myMana = 999'999.f; }
	if (ImGui::Button("Reset hp/mana")) { player->myHealth = player->myHealthMax;	player->myMana = player->myManaMax; }

	ImGui::Separator();
	ImGui::PushID(1337);
	ImGui::Text("Toggle Ability Available:");
	if (ImGui::Button("All"))
	{
		player->myArmorUnlocked = true;
		player->myAoEDoTUnlocked = true;
		player->myTeleportUnlocked = true;
		player->myUltimateUnlocked = true;
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 1);
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 2);
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 3);
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 4);
	}
	if (ImGui::Button("Mage Armor"))
	{
		player->myArmorUnlocked = true;
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 1);
	}
	if (ImGui::Button("AoEDoT"))
	{
		player->myAoEDoTUnlocked = true;
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 2);
	}
	if (ImGui::Button("Teleport"))
	{
		player->myTeleportUnlocked = true;
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 3);
	}
	if (ImGui::Button("Ultimate"))
	{
		player->myUltimateUnlocked = true;
		myCtx->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, 4);
	}
	ImGui::PopID();

	ImGui::Separator();
	ImGui::PushID(1338);
	ImGui::Text("Reset Ability CD:");
	if (ImGui::Button("All"))
	{
		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_Armor");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_Armor");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_2");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_2");

		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_AoEDoT");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_AoEDoT");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_3");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_3");

		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_Teleport");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_Teleport");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_4");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_4");

		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_Ultimate");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_Ultimate");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_5");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_5");
	}
	if (ImGui::Button("Mage Armor"))
	{
		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_Armor");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_Armor");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_2");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_2");
	}
	if (ImGui::Button("AoEDoT"))
	{
		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_AoEDoT");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_AoEDoT");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_3");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_3");
	}
	if (ImGui::Button("Teleport"))
	{
		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_Teleport");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_Teleport");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_4");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_4");
	}
	if (ImGui::Button("Ultimate"))
	{
		myCtx->myTimeHandler->InvokeTimer("Player_Cooldown_Ultimate");
		myCtx->myTimeHandler->RemoveTimer("Player_Cooldown_Ultimate");
		myCtx->myTimeHandler->InvokeTimer("Cooldown_VFX_5");
		myCtx->myTimeHandler->RemoveTimer("Cooldown_VFX_5");
	}
	ImGui::PopID();




	ImGui::Separator();
	if (ImGui::TreeNode("PlayerSnap"))
	{

		if (ImGui::Button("SaveToFile"))
		{
			SaveToJson();
		}
		if (ImGui::Button("ReadFromFile"))
		{
			ReadJson();
		}

		ImGui::InputText("Name", &myCurrentName);
		if (ImGui::Button("AddSegment"))
		{
			if (!myPlayerSnapPos.count(myCurrentName))
			{
				myPlayerSnapPos.insert({ myCurrentName, std::vector<PlayerSnapPos>() });
				myCurrentName = "";
			}
		}

		if (ImGui::Button("RemoveSegment"))
		{
			if (myPlayerSnapPos.count(myCurrentName))
			{
				myPlayerSnapPos.erase(myCurrentName);
				myCurrentName = "";
			}
		}

		for (auto& pair : myPlayerSnapPos)
		{
			if (ImGui::TreeNode(pair.first.c_str()))
			{
				ImGui::InputText((pair.first + "Name").c_str(), &myCurrentName);
				if (ImGui::Button("AddSnapPos"))
				{
					PlayerSnapPos PSP;
					PSP.myPosition = scene.GetComponent<TransformComponent>(playerId)->myPosition;
					PSP.myName = myCurrentName;
					pair.second.push_back(PSP);

					myCurrentName = "";

				}
				if (ImGui::Button("RemoveSnapPos"))
				{
					for (size_t i = 0; i < pair.second.size(); i++)
					{
						if (pair.second[i].myName == myCurrentName)
						{
							pair.second.erase(pair.second.begin() + i);
							myCurrentName = "";
							break;
						}
					}

				}
				if (ImGui::Button("SetSnap"))
				{
					for (size_t i = 0; i < pair.second.size(); i++)
					{
						if (pair.second[i].myName == myCurrentName)
						{
							pair.second[i].myPosition = scene.GetComponent<TransformComponent>(playerId)->myPosition;
							break;
						}
					}
				}

				ImGui::Separator();

				for (auto& snapPos : pair.second)
				{
					if (ImGui::Button(snapPos.myName.c_str()))
					{
						scene.GetComponent<TransformComponent>(playerId)->myPosition = snapPos.myPosition;
						break;
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}



}

void ISTE::PlayerTool::DrawAnimBlendTool()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	EntityID playerId = scene.GetPlayerId();
	PlayerBehaviour* player = scene.GetComponent<PlayerBehaviour>(playerId);
	if (player == nullptr)
	{
		ImGui::Text("No Player Detected");
		return;
	}
	
	ImGui::DragFloat("Idle->Move##FromIdle",		&player->myAnimationLerpData.myIdleToMoveSpeed, 0.1f); 
	ImGui::DragFloat("Move->Idle##FromMove",		&player->myAnimationLerpData.myMoveToIdleSpeed, 0.1f); 
	ImGui::DragFloat("Any->Idle##FromAny",			&player->myAnimationLerpData.myAnyToIdleSpeed, 0.1f);
	ImGui::DragFloat("Any->Move##FromAny",			&player->myAnimationLerpData.myAnyToMoveSpeed, 0.1f);
	ImGui::DragFloat("Any->Primary##FromAny",		&player->myAnimationLerpData.myAnyToPrimaryAttackSpeed, 0.1f);
	ImGui::DragFloat("Any->Secondary##FromAny",		&player->myAnimationLerpData.myAnyToSecondaryAttackSpeed, 0.1f);
	ImGui::DragFloat("Any->Spell##FromAny",			&player->myAnimationLerpData.myAnyToSpellSpeed, 0.1f);
	ImGui::DragFloat("Any->Dead##FromAny",			&player->myAnimationLerpData.myAnyToDeadSpeed, 0.1f);
	ImGui::DragFloat("Any->Bag##FromAny",			&player->myAnimationLerpData.myAnyToBagSpeed, 0.1f);

	if (ImGui::Button("Save"))
		SaveAnimBlendDataToJson();
}

void ISTE::PlayerTool::SaveToJson()
{
	std::string saveString = "../Assets/MiscData/PSP.json";
	nlohmann::json json;

	size_t counter = 0;
	for (auto& pair : myPlayerSnapPos)
	{
		json[counter]["SegmentName"] = pair.first;

		nlohmann::json segmentData;
		for (size_t i = 0; i < pair.second.size(); i++)
		{
			segmentData[i]["SnapName"] = pair.second[i].myName;
			segmentData[i]["Pos"]["X"] = pair.second[i].myPosition.x;
			segmentData[i]["Pos"]["Y"] = pair.second[i].myPosition.y;
			segmentData[i]["Pos"]["Z"] = pair.second[i].myPosition.z;
		}

		json[counter]["SegmentData"] = segmentData;

		counter++;
	}

	std::ofstream stream(saveString);

	stream << json;

	stream.close();


}

void ISTE::PlayerTool::SaveAnimBlendDataToJson()
{ 
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	EntityID playerId = scene.GetPlayerId();
	PlayerBehaviour* player = scene.GetComponent<PlayerBehaviour>(playerId);
	std::string saveString = "../Assets/Json/PlayerAnimBlendData.json";
	nlohmann::json json;

	json["MoveTo"]["Idle"]		= player->myAnimationLerpData.myMoveToIdleSpeed; 
	json["IdleTo"]["Move"]		= player->myAnimationLerpData.myIdleToMoveSpeed; 

	json["AnyTo"]["Idle"]		= player->myAnimationLerpData.myAnyToIdleSpeed;
	json["AnyTo"]["Move"]		= player->myAnimationLerpData.myAnyToMoveSpeed;
	json["AnyTo"]["Primary"]	= player->myAnimationLerpData.myAnyToPrimaryAttackSpeed;
	json["AnyTo"]["Secondary"]	= player->myAnimationLerpData.myAnyToSecondaryAttackSpeed;
	json["AnyTo"]["Spell"]		= player->myAnimationLerpData.myAnyToSpellSpeed;
	json["AnyTo"]["Dead"]		= player->myAnimationLerpData.myAnyToDeadSpeed;
	json["AnyTo"]["Bag"]		= player->myAnimationLerpData.myAnyToDeadSpeed;

	std::ofstream stream(saveString); 
	stream << json; 
	stream.close();
}

void ISTE::PlayerTool::ReadJson()
{
	myPlayerSnapPos.clear();
	std::string saveString = "../Assets/MiscData/PSP.json";
	nlohmann::json json;

	std::ifstream stream(saveString);
	stream >> json;
	stream.close();

	size_t segments = json.size();

	for (size_t segment = 0; segment < segments; segment++)
	{
		std::string segmentName = json[segment]["SegmentName"];
		myPlayerSnapPos.insert({ segmentName, std::vector<PlayerSnapPos>() });

		nlohmann::json segmentData = json[segment]["SegmentData"];
		size_t snaps = segmentData.size();

		for (size_t snap = 0; snap < snaps; snap++)
		{
			PlayerSnapPos PSP;
			PSP.myName = segmentData[snap]["SnapName"];
			PSP.myPosition.x = segmentData[snap]["Pos"]["X"];
			PSP.myPosition.y = segmentData[snap]["Pos"]["Y"];
			PSP.myPosition.z = segmentData[snap]["Pos"]["Z"];

			myPlayerSnapPos[segmentName].push_back(PSP);
		}

	}
}
