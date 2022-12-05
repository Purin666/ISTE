
#include "State_FadeOut.h"

#include "ISTE/Context.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Events/EventHandler.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/CU/Database.h"
#include "ISTE/CU/UtilityFunctions.hpp"

void ISTE::State_FadeOut::Init()
{
	SetStateFlags(StateFlags::StateFlags_PassthruRender);

	myFade = Sprite2DRenderCommand();
	myLoad = Sprite2DRenderCommand();
	myDuration = 1.f;
	myElapsedTime = 0.f;;

	TextureManager* txrMgr = myCtx->myTextureManager;

	myFade.myTextureId[ALBEDO_MAP] = txrMgr->LoadTexture(L"../Assets/Sprites/fadeTexture.dds");

	myFade.myColor = CU::Vec4f(1, 1, 1, 0);
	myFade.myPosition = CU::Vec2f(0, 0);
	myFade.myScale = CU::Vec2f(2, 2);

	myFade.myUVStart = CU::Vec2f(0, 0);
	myFade.myUVEnd = CU::Vec2f(1, 1);
	myFade.myUVOffset = CU::Vec2f(0, 0);
	myFade.myUVScale = CU::Vec2f(1, 1);

	myFade.mySamplerState = SamplerState::ePoint;

	myLoad.myTextureId[ALBEDO_MAP] = txrMgr->LoadTexture(L"../Assets/Sprites/Logos/tga_w.dds");

	myLoad.myColor = CU::Vec4f(1, 1, 1, 0);
	myLoad.myPosition = CU::Vec2f(0, 0);
	myLoad.myScale = CU::Vec2f(1, 1);

	myLoad.myUVStart = CU::Vec2f(0, 0);
	myLoad.myUVEnd = CU::Vec2f(1, 1);
	myLoad.myUVOffset = CU::Vec2f(0, 0);
	myLoad.myUVScale = CU::Vec2f(1, 1);

	myCtx->myEventHandler->InvokeEvent(EventType::LevelCompleted, myCtx->mySceneHandler->GetActiveSceneIndex());
}

void ISTE::State_FadeOut::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	myFade.myColor.a += aDeltaTime / myDuration;
	myFade.myColor.a = CU::Min(myFade.myColor.a, 1.f);

	if ((myDuration + 0.2f) < myElapsedTime)
	{
		myFade.myColor.a = 1.f;

		SetStateFlags(StateFlags::StateFlags_None);

		// Get
		const std::string stackToLoad = myCtx->myGenericDatabase->Get<std::string>("StackOfStatesToLoadAfterFadeOut");
		const size_t sceneToLoad = myCtx->myGenericDatabase->Get<size_t>("SceneToLoadAfterFadeOut");
		const bool shouldLoad = sceneToLoad != (size_t)myCtx->mySceneHandler->GetActiveSceneIndex();

		bool shouldReload = false;
		myCtx->myGenericDatabase->TryGet<bool>("ReloadScene", shouldReload);

		// Load
		myCtx->myStateManager->LoadStack(stackToLoad);
		if (shouldLoad) myCtx->mySceneHandler->LoadScene(sceneToLoad, true);
		if (shouldReload)
		{
			myCtx->myGenericDatabase->SetValue("ReloadScene", false);
			myCtx->mySceneHandler->LoadScene(sceneToLoad);
		}

		return;
	}
}

void ISTE::State_FadeOut::Render()
{
	SpriteDrawerSystem* drawer = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	
	drawer->Add2DSpriteRenderCommand(myFade);

	if (myDuration < myElapsedTime)
		drawer->Add2DSpriteRenderCommand(myLoad);
}