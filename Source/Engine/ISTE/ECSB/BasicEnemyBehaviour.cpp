#include "BasicEnemyBehaviour.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "PlayerBehaviour.h"

#include "ISTE/CU/MemTrack.hpp"

void ISTE::BasicEnemyBehaviour::Init()
{
	myActiveScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();
	TransformComponent* aTransform = myActiveScene->GetComponent<TransformComponent>(myHostId);
	if (aTransform != nullptr)
	{
		myTargetPosition = aTransform->myPosition;
	}
}

void ISTE::BasicEnemyBehaviour::Update(float aDeltaTime)
{
	TransformComponent* transform = myActiveScene->GetComponent<TransformComponent>(myHostId);
	if (myActiveScene->GetComponent<PlayerBehaviour>(myActiveScene->GetPlayerId()) != nullptr)
	{
		TransformComponent* playerTransform = myActiveScene->GetComponent<TransformComponent>(myActiveScene->GetPlayerId());
		if (playerTransform != nullptr)
		{
			if ((transform->myPosition - playerTransform->myPosition).Length() > mySightRange)
			{
				myTargetPosition = playerTransform->myPosition;
			}

		}
	}

	CU::Vec3<float> startEndDifference = transform->myPosition - myTargetPosition;
	if (startEndDifference.Length() > mySpeed * aDeltaTime)
	{
		transform->myPosition += startEndDifference.GetNormalized() * (mySpeed * aDeltaTime);
	}
	else
	{
		transform->myPosition = myTargetPosition;
	}



}
