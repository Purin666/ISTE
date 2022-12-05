#include "MaterialEdit.h"

#include <d3d11.h>

#include "imgui/imgui.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"
#include "Commands/CommandFunctions.h"
//#include "../Engine/ISTE/Context.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "ISTE/Helper/StringCast.h"

ISTE::MaterialEdit::MaterialEdit()
{
}

ISTE::MaterialEdit::~MaterialEdit()
{
}

void ISTE::MaterialEdit::Draw(ISTE::Scene* aScenePointer, Entity anEntity)
{
	ISTE::MaterialComponent* material = aScenePointer->GetComponent<ISTE::MaterialComponent>(anEntity.myId);
	ISTE::ModelComponent* mComp = aScenePointer->GetComponent<ISTE::ModelComponent>(anEntity.myId);
	if (material == nullptr)
		return;

	std::vector<std::vector<std::string>> stringPaths;
	ImGui::Text("Materials");
	if (mComp != nullptr)
	{
		if (mComp->myModelId == ModelID(-1))
		{
			ImGui::Separator(); 
			return;
		}
		size_t meshCount = Context::Get()->myModelManager->GetMeshCount(mComp->myModelId);
		MaterialLoop(material->myTextures, meshCount);  
	}

	ISTE::DecalComponent* dComp = aScenePointer->GetComponent<ISTE::DecalComponent>(anEntity.myId);
	if (dComp != nullptr)
	{ 
		MaterialLoop(material->myTextures, 1); 
	}



	//for (size_t i = 0; i < MaxMeshCount; i++)
	//{
	//	std::vector<std::string> textures;
	//	for (size_t j = 0; j < MaxMaterialCount; j++)
	//	{
	//		if (material->myTextures[i][j] != TextureID(-1))
	//		{
	//			std::wstring tPath = Context::Get()->myTextureManager->GetTexture(material->myTextures[i][j]).myFilePath;
	//			textures.push_back(std::string(tPath.begin(), tPath.end()));
	//		}
	//	}
	//	if (textures.size() > 0)
	//	{
	//		stringPaths.push_back(textures);
	//		continue;
	//	}
	//	break;
	//}
	//if (stringPaths.size() == 0)
	//{
	//	return;
	//}
	//if (ImGui::BeginCombo("Meshes", std::to_string(myCurrentMesh).c_str()))
	//{
	//	for (int i = 0; i < stringPaths.size(); i++)
	//	{
	//		if (ImGui::Selectable(std::to_string(i).c_str()))
	//		{
	//			myCurrentMesh = i;
	//		}
	//	}
	//	ImGui::EndCombo();
	//}
	//
	//
	//ImGui::Text("Materials");
	//ImGui::Spacing();
	//for (int i = 0; i < stringPaths[myCurrentMesh].size(); i++)
	//{
	//	if (ImGui::InputText(("##" + std::to_string(i)).c_str(), &stringPaths[myCurrentMesh][i], ImGuiInputTextFlags_EnterReturnsTrue))
	//	{
	//		material->myTextures[myCurrentMesh][i] = Context::Get()->myTextureManager->LoadTexture(StringCast<std::wstring>(stringPaths[myCurrentMesh][i]).c_str());
	//	}
	//}
	ImGui::Separator();
	ImGui::Spacing();
}

void ISTE::MaterialEdit::MaterialLoop(TextureID aTexList[MAX_MESH_COUNT][MAX_MATERIAL_COUNT], const size_t meshCount)
{
	auto& texList = Context::Get()->myTextureManager->GetTextureList();
	ID3D11ShaderResourceView* srv = nullptr;
	std::string name;
	std::string header;
	std::string path;
	std::wstring pathw;
	for (size_t i = 0; i < meshCount; i++)
	{
		header = "Material " + std::to_string(i);
		if (ImGui::CollapsingHeader(header.c_str()))
		{
			for (size_t j = 0; j < MAX_MATERIAL_COUNT; j++)
			{
				ImGui::Separator();

				if (aTexList[i][j] != TextureID(-1))
				{
					auto& T = texList.GetByIterator(aTexList[i][j]);;
					srv = T.mySRV.Get();
					name = std::string(T.myFilePath.begin(), T.myFilePath.end());
				}
				else
				{
					srv = nullptr;
					name = "Texture Not Bound";
				}
				ImGui::Image(srv, { 64,64 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds", 0))
					{
						path = *(std::string*)payload->Data;
						pathw = std::wstring(path.begin(), path.end());
						if (j == ALBEDO_MAP)
						{
							aTexList[i][j] = Context::Get()->myTextureManager->LoadTexture(pathw, true);
							continue;
						}
						aTexList[i][j] = Context::Get()->myTextureManager->LoadTexture(pathw, false);

					}
					ImGui::EndDragDropTarget();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".DDS", 0))
					{
						path = *(std::string*)payload->Data;
						pathw = std::wstring(path.begin(), path.end());
						if (j == ALBEDO_MAP)
						{
							aTexList[i][j] = Context::Get()->myTextureManager->LoadTexture(pathw, true);
							continue;
						}
						aTexList[i][j] = Context::Get()->myTextureManager->LoadTexture(pathw, false);

					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine(); ImGui::Text(name.c_str());
			}
		}
	}
}
