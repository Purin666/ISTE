#include "MaterialComponentCRAW.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Context.h"
#include "ISTE/Helper/StringCast.h"
nlohmann::json ISTE::MaterialComponentCRAW::Write(void* aComponent, EntityID aEntity)
{
	nlohmann::json json;

	MaterialComponent* mC = (MaterialComponent*)aComponent;

	for (size_t i = 0; i < MAX_MESH_COUNT; i++)
	{
		for (size_t j = 0; j < MAX_MATERIAL_COUNT; j++)
		{ 
			if (mC->myTextures[i][j] != TextureID(-1))
			{
				std::wstring tPath = Context::Get()->myTextureManager->GetTexture(mC->myTextures[i][j]).myFilePath;
				std::string path(tPath.begin(), tPath.end());
				json["Mesh"][i]["Material"][j] = path;
			}
			else
			{
				break;
			}
		}
	}

	return json;

}
void ISTE::MaterialComponentCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
{
	MaterialComponent* mC = (MaterialComponent*)aComponent;
	std::string path;
	size_t idxI = 0;
	size_t idxJ = 0;
	for (const nlohmann::json& i : aJson["Mesh"])
	{
		idxJ = 0;
		for (const nlohmann::json& j : i["Material"])
		{
			path = j.get<std::string>();
			if (!path.empty())
			{
				mC->myTextures[idxI][idxJ] = Context::Get()->myTextureManager->LoadTexture(StringCast<std::wstring>(path).c_str());
			}
			idxJ++;
		}
		idxI++;
	}
}