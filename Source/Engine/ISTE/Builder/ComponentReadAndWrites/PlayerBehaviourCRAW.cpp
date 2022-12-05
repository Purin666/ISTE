#include "PlayerBehaviourCRAW.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/CU/ReadWrite.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/ECSB/PlayerBehaviour.h"

nlohmann::json ISTE::PlayerBehaviourCRAW::Write(void* aComponent, EntityID aEntity)
{
	return nlohmann::json();
}

void ISTE::PlayerBehaviourCRAW::Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson)
{

	PlayerBehaviour* pB = (PlayerBehaviour*)aComponent;
	const int index = Context::Get()->mySceneHandler->GetActiveSceneIndex();
	const std::string camPath = "../Assets/MiscData/PlayerCamera/PlayerCamera_" + std::to_string(index) + ".cam";
	if (ReadWrite::LoadFile(camPath))
	{
		TransformComponent& camera = pB->GetCameraPositioning();

		CU::Vec3f rot;
		ReadWrite::Read<CU::Vec3f>(rot);
		camera.myEuler.SetRotation(rot);

		CU::Vec3f pos;
		ReadWrite::Read<CU::Vec3f>(pos);
		camera.myPosition = pos;

		float fov = 0;
		ReadWrite::Read<float>(fov);

		pB->GetCameraPositioning().myScale.x = fov;
		pB->GetCamera().SetPerspectiveProjection(fov, Context::Get()->myWindow->GetResolution(), 0.5f, 5000);

		ReadWrite::Flush();
	}

	//

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();


	scene.SetPlayerId(aEntity);
}
