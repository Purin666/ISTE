#include "BossTrigger.h"

#include <iostream>
#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/Camera.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/ECSB/PlayerBehaviour.h"
#include "ISTE/ECSB/BossBobBehaviour.h"

#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/CU/UtilityFunctions.hpp"

#include "ISTE/CU/MemTrack.hpp"

void ISTE::BossTrigger::Init()
{
	myScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();

	myIsBossActive = false;
	myUnderTransition = false;
	myUseRoar = false;
}

void ISTE::BossTrigger::Update(float aDeltaTime)
{
	if (myIsBossActive)
		return;
	 
	

	if (myUnderTransition)
	{
		myPostRoarWaitTimer += aDeltaTime;
		
		if (myPostRoarWaitTimer <= ((myPanTimer) + myWaitTimer) - 0.22)
		{
			float t = 0;
			t = myRoarBlurTimer - (myRoarBlurTimer * myRoarBlurTimer * myRoarBlurTimer);//((sin(myRoarBlurTimer * 3.14 * 3.4 - 1.4) + 1) / 2.5) + (myRoarBlurTimer * 0.6f) - myRoarBlurTimer * myRoarBlurTimer * myRoarBlurTimer;
			myRoarBlurTimer += (aDeltaTime) / (myPanTimer + myWaitTimer);
			CU::Clamp(0.f, 1.f, t);
			Context::Get()->myGraphicsEngine->GetRadialBlur().GetBufferData().myBlurStrength = t * 6.5f;

			if (myPostRoarWaitTimer - myPanTimer >= (myPanTimer + myWaitTimer) )
			{ 
				Context::Get()->myGraphicsEngine->GetAberrationEffect().GetBufferData().myRadialStrength.r = t * 9;
			}

			if (myPostRoarWaitTimer >= (myPanTimer + myWaitTimer)  )
			{
				Context::Get()->myGraphicsEngine->GetAberrationEffect().GetBufferData().myRadialStrength.b = -t * 4.5;
			}
		}
		else
		{
			Context::Get()->myGraphicsEngine->GetAberrationEffect().SetEnabled(false);
			Context::Get()->myGraphicsEngine->GetRadialBlur().SetEnabled(false);

			Context::Get()->myTimeHandler->RemoveTimer("BossTriggerRoarShake"); 
		}
		return;
	}

	TransformComponent* transform = myScene->GetComponent<TransformComponent>(myHostId);
	TransformComponent* pTransfrom = myScene->GetComponent<TransformComponent>(myScene->GetPlayerId());

	float length = (pTransfrom->myPosition - transform->myPosition).Length();

	if (length <= myRadius)
	{
		if (!myUseCamera)
		{
			myScene->GetComponent<BossBobBehaviour>(myScene->GetBossId())->SetIsActive(true);
			myScene->GetComponent<BossBobBehaviour>(myScene->GetBossId())->SetAllSpawnPools(mySpawnPool);
			myIsBossActive = true;
			return;
		}

		PlayerBehaviour* player = myScene->GetComponent<PlayerBehaviour>(myScene->GetPlayerId());
		player->SetIsActive(false);
		CU::Euler* playerEuler = &player->GetCameraPositioning().myEuler;

		const float xEuler = myDegreesToRotate / myPanTimer;
		const float savedX = playerEuler->GetAngles().x;

		myUnderTransition = true;



		CountDown cameraTimer;
		cameraTimer.name = "BossTriggerCameraTimer";
		cameraTimer.duration = myPanTimer + myWaitTimer + myPostRoarWaitDuration;
		cameraTimer.callback = [this, player, savedX, playerEuler, xEuler]()
		{
			UpdateTimer cameraPan;
			cameraPan.name = "BossTriggerCameraPan";
			cameraPan.duration = myPanTimer;
			cameraPan.callback = [this, player, xEuler, playerEuler]()
			{ 
				const float x = playerEuler->GetAngles().x + xEuler * Context::Get()->myTimeHandler->GetDeltaTime();
				playerEuler->SetRotation(x, playerEuler->GetAngles().y, playerEuler->GetAngles().z);
				CU::Vec3f pos = player->GetCamera().GetPosition();
				player->GetCamera().GetTransformNonConst() = playerEuler->GetRotationMatrix();
				player->GetCamera().GetTransformNonConst().GetTranslationV3() = pos;


			};
			Context::Get()->myTimeHandler->AddTimer(cameraPan);

			CountDown cameraTimer;
			cameraTimer.name = "BossTriggerCameraTimer";
			cameraTimer.duration = myPanTimer;
			cameraTimer.callback = [this, player]()
			{
				player->SetIsActive(true);
				Context::Get()->myGenericDatabase->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
				Context::Get()->myGenericDatabase->SetValue<size_t>("SceneToLoadAfterFadeOut", 6);
				Context::Get()->myStateManager->PushState(ISTE::StateType::FadeOut);


				// doing fade out/in instead of instant loading scene
				//Context::Get()->mySceneHandler->LoadScene(Context::Get()->mySceneHandler->GetActiveSceneIndex() + 1, true);
				//Context::Get()->myStateManager->LoadStack("PlayBoss");
			};
			Context::Get()->myTimeHandler->AddTimer(cameraTimer);
		};
		Context::Get()->myTimeHandler->AddTimer(cameraTimer);

		UpdateTimer cameraPan;
		cameraPan.name = "BossTriggerCameraPan";
		cameraPan.duration = myPanTimer;
		cameraPan.callback = [this, player, xEuler, playerEuler]()
		{ 
			const float x = playerEuler->GetAngles().x - xEuler * Context::Get()->myTimeHandler->GetDeltaTime();
			playerEuler->SetRotation(x, playerEuler->GetAngles().y, playerEuler->GetAngles().z);
			CU::Vec3f pos = player->GetCamera().GetPosition();
			player->GetCamera().GetTransformNonConst() = playerEuler->GetRotationMatrix();
			player->GetCamera().GetTransformNonConst().GetTranslationV3() = pos;

			
			if (Context::Get()->myTimeHandler->GetTime("BossTriggerCameraPan") >= ((myPanTimer / 2.f)) && !myUseRoar)
			{
				myScene->GetComponent<BossBobBehaviour>(myScene->GetBossId())->DoRoarEvent();
				myUseRoar = true;
				//t = 0.1;//sin(myRoarBlurTimer * 3.14);
				Context::Get()->myGraphicsEngine->GetRadialBlur().SetEnabled(true);
				Context::Get()->myGraphicsEngine->GetAberrationEffect().SetEnabled(true);
				StepTimer cameraShake;
				cameraShake.name = "BossTriggerRoarShake";
				cameraShake.delay = 0.075f;
				cameraShake.callback = [this, player, pos]() {

					myRoarBlurTimer = (0.075f * 2.1) / (myPanTimer + myWaitTimer);
					myShakeLerpTimer += myRoarBlurTimer;
					float t = 0; 
					t = myShakeLerpTimer - (myShakeLerpTimer * myShakeLerpTimer * myShakeLerpTimer);
					CU::Clamp(0.f, 1.f, t);
					t *= 2.6;

					float randomY = CU::GetRandomFloat(-0.5f, 0.5f);
					float randomX = CU::GetRandomFloat(-0.5f, 0.5f);

					CU::Vec3f cameraShakePos = pos;

					cameraShakePos.y += randomY * t * 1.4f;
					cameraShakePos.x += randomX * t * 1.4f;

					player->GetCamera().GetTransformNonConst().GetTranslationV3() = cameraShakePos;
				};
				Context::Get()->myTimeHandler->AddTimer(cameraShake);
			}

			
		};
		Context::Get()->myTimeHandler->AddTimer(cameraPan);
	}
}