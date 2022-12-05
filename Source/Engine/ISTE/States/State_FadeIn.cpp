
#include "State_FadeIn.h"

#include "ISTE/Context.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"

void ISTE::State_FadeIn::Init()
{
	SetStateFlags(StateFlags_PassthruUpdate | StateFlags_AlwaysRender);

	myFade = Sprite2DRenderCommand();

	myDuration = 1.f;
	myElapsedTime = 0.f;;

	TextureManager* txrMgr = myCtx->myTextureManager;

	myFade.myTextureId[ALBEDO_MAP] = txrMgr->LoadTexture(L"../Assets/Sprites/fadeTexture.dds");

	myFade.myColor = CU::Vec4f(1, 1, 1, 1);
	myFade.myPosition = CU::Vec2f(0, 0);
	myFade.myScale = CU::Vec2f(2, 2);

	myFade.myUVStart = CU::Vec2f(0, 0);
	myFade.myUVEnd = CU::Vec2f(1, 1);
	myFade.myUVOffset = CU::Vec2f(0, 0);
	myFade.myUVScale = CU::Vec2f(1, 1);

	StopWatch stopWatch;
	stopWatch.name = "Played_Time_Current_Level";
	myCtx->myTimeHandler->AddTimer(stopWatch);
}

void ISTE::State_FadeIn::Update(const float aDeltaTime)
{
	if (mySkipFirstFrame) // first delta time is about 3.f so just quick thing for that
	{
		mySkipFirstFrame = false;
		return;
	}

	myElapsedTime += aDeltaTime;

	myFade.myColor.a -= aDeltaTime / myDuration;

	if (myDuration < myElapsedTime)
	{
		myFade.myColor.a = 0.f;
		myCtx->myStateManager->PopState();
		return;
	}
}

void ISTE::State_FadeIn::Render()
{
	myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>()->Add2DSpriteRenderCommand(myFade);
}