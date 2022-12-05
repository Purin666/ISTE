#include "ComponentReadAndWrite.h"
#include "ModelComponentCRAW.h"

#include "ISTE\Context.h"
#include "ISTE\Graphics\Resources\ModelManager.h"
#include "ISTE\Graphics\ComponentAndSystem\ModelComponent.h"


nlohmann::json ISTE::ModelComponentCRAW::Write(void* aComponent, EntityID)
{
	ModelComponent* mCmp = (ModelComponent*)aComponent;
	if (mCmp == nullptr)
		return {};
	nlohmann::json json;

	if (mCmp->myModelId == TextureID(-1))
		return json;
	std::string path = Context::Get()->myModelManager->GetModel(mCmp->myModelId)->myFilePath;


	json["FBX"] = path;

	return json;
}

void ISTE::ModelComponentCRAW::Read(void* aComponent, EntityID, nlohmann::json& aJson)
{

	ModelComponent* mCmp = new (aComponent) ModelComponent();

	std::string path = aJson["FBX"];

	ModelManager::ModelLoadResult result = Context::Get()->myModelManager->LoadModel(path);

	mCmp->myModelId = result.myValue;

}
