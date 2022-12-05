
#include "State_SplashScreen.h"

#include "ISTE/Context.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"
#include "ISTE/UI/UIHandler.h"

#include "ISTE/CU/UtilityFunctions.hpp"

ISTE::State_SplashScreen::~State_SplashScreen()
{
	for (size_t i = 0; i < 5; i++)
	{
		myCtx->mySprite2DParticleHandler->DeactivateEmitter(myEmitterIds[i]);
	}
}

void ISTE::State_SplashScreen::Init()
{
	SetStateFlags(StateFlags_PassthruRender);

	myBackground = Sprite2DRenderCommand();
	myLogoIndex = 0;
	myElapsedTime = -0.5f;
	myLogoDuration = 3.f;

	myCtx->myUIHandler->EnableMouseEmitter(false);

	const float logoSize = (1.f / 3.f) * 2.f;
	const CU::Vec2Ui res = myCtx->myWindow->GetResolution();
	const float ratio = (float)res.x / (float)res.y;
	
	TextureManager* txrMgr = myCtx->myTextureManager;

	myBackground.myTextureId[ALBEDO_MAP] = txrMgr->LoadTexture(L"../Assets/Sprites/fadeTexture.dds");

	myBackground.myColor	= CU::Vec4f(1, 1, 1, 1);
	myBackground.myPosition = CU::Vec2f(0, 0);
	myBackground.myScale	= CU::Vec2f(2, 2);

	myBackground.myUVStart	= CU::Vec2f(0, 0);
	myBackground.myUVEnd	= CU::Vec2f(1, 1);
	myBackground.myUVOffset = CU::Vec2f(0, 0);
	myBackground.myUVScale	= CU::Vec2f(1, 1);

	myLogos[0].myTextureId[ALBEDO_MAP] = txrMgr->LoadTexture(L"../Assets/Sprites/Logos/logoTGA.dds");

	myLogos[0].myColor		= CU::Vec4f(1, 1, 1, 0);
	myLogos[0].myPosition	= CU::Vec2f(0, 0);
	myLogos[0].myScale		= CU::Vec2f(logoSize, logoSize * ratio * 0.5f);

	myLogos[0].myUVStart	= CU::Vec2f(0, 0);
	myLogos[0].myUVEnd		= CU::Vec2f(1, 1);
	myLogos[0].myUVOffset	= CU::Vec2f(0, 0);
	myLogos[0].myUVScale	= CU::Vec2f(1, 1);	

	myLogos[1].myTextureId[ALBEDO_MAP] = txrMgr->LoadTexture(L"../Assets/Sprites/Logos/dsp.dds");

	myLogos[1].myColor		= CU::Vec4f(2, 2, 2, 0);
	myLogos[1].myPosition	= CU::Vec2f(0, 0);
	myLogos[1].myScale		= CU::Vec2f(logoSize, logoSize * ratio);
	myLogos[1].myUVStart	= CU::Vec2f(0, 0);

	myLogos[1].myUVEnd		= CU::Vec2f(1, 1);
	myLogos[1].myUVOffset	= CU::Vec2f(0, 0);
	myLogos[1].myUVScale	= CU::Vec2f(1, 1);

	//myEmitterIds[0] = myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_Red_Small");
	//myEmitterIds[1] = myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_Red_Big");
	//myEmitterIds[2] = myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_White_Small");
	//myEmitterIds[3] = myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_White_Big");

	myEmitterIds[0] = myCtx->mySprite2DParticleHandler->SpawnEmitter("SplashScreen_Sparks_1_Left");
	myEmitterIds[1] = myCtx->mySprite2DParticleHandler->SpawnEmitter("SplashScreen_Sparks_1_Right");
	myEmitterIds[2] = myCtx->mySprite2DParticleHandler->SpawnEmitter("SplashScreen_Sparks_2_Left");
	myEmitterIds[3] = myCtx->mySprite2DParticleHandler->SpawnEmitter("SplashScreen_Sparks_2_Right");
	//myEmitterIds[4] = myCtx->mySprite2DParticleHandler->SpawnEmitter("SplashScreen_Smoke");
}

void ISTE::State_SplashScreen::Update(const float aTimeDelta)
{
	// first delta time is quite big so just quick thing for that
	if (aTimeDelta > 0.1f)
		return;

	myElapsedTime += aTimeDelta;

	const bool skip = myCtx->myInputHandler->IsKeyDown(VK_RETURN) ||
					  myCtx->myInputHandler->IsKeyDown(VK_SPACE);
	
	if (skip || myLogoIndex >= 2)
	{
		myCtx->myStateManager->PopState();
		myCtx->myUIHandler->EnableMouseEmitter(true);
		return;
	}
	// std::fmodf(myElapsedTime, myLogoDuration) / myLogoDuration
	myLogos[myLogoIndex].myColor.a = CU::EaseInOut(CU::Max(0.f, myElapsedTime / myLogoDuration));
	if (myElapsedTime > myLogoDuration)
	{
		myElapsedTime = -0.2f;
		++myLogoIndex;
	}
}

void ISTE::State_SplashScreen::Render()
{
	SpriteDrawerSystem* drawer = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	
	drawer->Add2DSpriteRenderCommand(myBackground);
	if (myLogoIndex < 2)
		drawer->Add2DSpriteRenderCommand(myLogos[myLogoIndex]);
}
