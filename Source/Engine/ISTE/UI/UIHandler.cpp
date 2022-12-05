#include "UIHandler.h"

#include "ISTE/Context.h"

#include "UIHelpers.h"
#include "ISTE/CU/UtilityFunctions.hpp"

#include "ISTE/CU/RayFromScreen.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/Audio/AudioHandler.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/Events/EventHandler.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"

#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"

#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/CU/Database.h"
#include "ISTE/CU/InputHandler.h"

#include "ISTE/Audio/AudioSourceSystem.h"

#include "Json/json.hpp"
#include <fstream>

#include "ISTE/CU/MemTrack.hpp"

constexpr float ASPECT_RATIO = 16.f / 9.f;

ISTE::UIHandler::~UIHandler()
{
}

void ISTE::UIHandler::Init()
{
	myCtx = Context::Get();
	
	InitMouse();

	InitGameplayTypes();
	InitCallbacks();
	LoadCallbacks();
	LoadTexturesAndFonts();

	//ShowCursor(false);
}

void ISTE::UIHandler::UpdateMouse()
{
	// mouse info
	ISTE::ScreenHelper helper;
	const POINT point = helper.GetMouseRelativePosition();
	const CU::Vec2Ui res = myCtx->myWindow->GetResolution();
	const float x = (float)point.x / res.x * 2 - 1;
	const float y = (float)point.y / res.y * 2 - 1;
	const CU::Vec2f mousePos(x, -y);
	const bool mouseDown = myCtx->myInputHandler->IsKeyDown(VK_LBUTTON);
	const bool mouseHeld = myCtx->myInputHandler->IsKeyHeldDown(VK_LBUTTON);
	const bool mouseUp = myCtx->myInputHandler->IsKeyUp(VK_LBUTTON);

	// move mouse sprite
	CU::Vec2f sPos = mousePos;
	sPos.x += myMouse.myScale.x;
	sPos.y -= myMouse.myScale.y;
	myMouse.myPosition = sPos;

	if (myMouseEmitterFlag)
	{
		if (mouseDown)
		{
			CU::Vec2f ePos = mousePos;
			ePos.x *= ASPECT_RATIO;
			myClickEmitter = myCtx->mySprite2DParticleHandler->SpawnEmitter("MouseClick", ePos);
		}
		if (mouseHeld)
		{
			CU::Vec2f ePos = mousePos;
			ePos.x *= ASPECT_RATIO;
			myCtx->mySprite2DParticleHandler->SetEmitterPosition(myClickEmitter, ePos);
		}
		if (mouseUp)
		{
			myCtx->mySprite2DParticleHandler->DeactivateEmitter(myClickEmitter);
			myClickEmitter = -1;
		}
		// move mouse emitter
		CU::Vec2f ePos = myMouse.myPosition;
		ePos.x *= ASPECT_RATIO;
		myCtx->mySprite2DParticleHandler->SetEmitterPosition(myMouseEmitter, ePos);
	}
}
void ISTE::UIHandler::EnableMouseEmitter(const bool aFlag)
{
	myMouseEmitterFlag = aFlag;
	if (!myMouseEmitterFlag)
	{
		// yeet mouse emitter
		myCtx->mySprite2DParticleHandler->SetEmitterPosition(myMouseEmitter, CU::Vec2f(10000, 10000));
		myCtx->mySprite2DParticleHandler->SetEmitterPosition(myClickEmitter, CU::Vec2f(10000, 10000));
	}
}
void ISTE::UIHandler::RenderMouse()
{
	SpriteDrawerSystem* drawer = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	drawer->Add2DSpriteRenderCommand(myMouse);
}

void ISTE::UIHandler::LoadTexturesAndFonts()
{
	for (auto& [name, ui] : myUIDatas)
	{
		for (auto& [name, image] : ui.myImages)
		{
			const std::wstring path(image.myPath.begin(), image.myPath.end());
			image.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(path).myValue;
		}
		for (auto& [name, text] : ui.myTexts)
		{
			text.myCommand.myFontID = myCtx->myTextureManager->LoadFont(text.myPath, text.mySize, text.myBorder);
		}
		for (auto& [name, button] : ui.myButtons)
		{
			const std::wstring path(button.myImage.myPath.begin(), button.myImage.myPath.end());
			button.myImage.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(path).myValue;
		}
		for (auto& [name, image] : ui.myGameplayImages)
		{
			const std::wstring path(image.myImage.myPath.begin(), image.myImage.myPath.end());
			image.myImage.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(path).myValue;
		}

		for (auto& [name, tooltip] : ui.myTooltips)
		{
			for (auto& image : tooltip.myImages)
			{
				const std::wstring path(image.myPath.begin(), image.myPath.end());
				image.myCommand.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(path).myValue;
			}
			for (auto& text : tooltip.myTexts)
			{
				text.myCommand.myFontID = myCtx->myTextureManager->LoadFont(text.myPath, text.mySize, text.myBorder);
			}
		}
	}
}

void ISTE::UIHandler::LoadCallbacks()
{
	for (auto& [name, ui] : myUIDatas)
	{
		for (auto& [name, button] : ui.myButtons)
		{
			button.myCallback = myCallbacks[button.myCallbackName];
		}
	}
}

void ISTE::UIHandler::InitGameplayTypes()
{
	myGameplayTypes[GameplayUIType::PlayerHealth] = "PlayerHealth";
	myGameplayTypes[GameplayUIType::PlayerMana] = "PlayerMana";
	myGameplayTypes[GameplayUIType::PlayerExperience] = "PlayerExperience";
	myGameplayTypes[GameplayUIType::BossHealth] = "BossHealth";
	myGameplayTypes[GameplayUIType::AbilityL] = "AbilityL";
	myGameplayTypes[GameplayUIType::AbilityR] = "AbilityR";
	myGameplayTypes[GameplayUIType::Ability1] = "Ability1";
	myGameplayTypes[GameplayUIType::Ability2] = "Ability2";
	myGameplayTypes[GameplayUIType::Ability3] = "Ability3";
	myGameplayTypes[GameplayUIType::Ability4] = "Ability4";
}

void ISTE::UIHandler::InitCallbacks()
{
	auto genericDatabase = Context::Get()->myGenericDatabase;
	auto stateManager = Context::Get()->myStateManager;
	auto audioHandler = Context::Get()->myAudioHandler;
	auto sceneHandler = Context::Get()->mySceneHandler;

	myCallbacks["Default"] = nullptr;
	myCallbacks["Exit_Game"] = [sm = stateManager]() {
		CountDown countDown;
		countDown.duration = 1.1f;
		countDown.callback = []() { PostQuitMessage(0); };
		Context::Get()->myTimeHandler->AddTimer(countDown);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::FadeOut);
	};

	myCallbacks["Play"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 1);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Main_Menu"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "MainMenu");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 0);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Restart"] = [db = genericDatabase, sm = stateManager, sh = sceneHandler]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", sh->GetActiveSceneIndex());
		db->SetValue("ReloadScene", true);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::FadeOut);
	};

	myCallbacks["Level_Select"] = [sm = stateManager]() { 
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::MenuLevelSelect); 
	};

	myCallbacks["Settings"]		= [sm = stateManager]() { 
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::MenuSettings);
	};

	myCallbacks["Stats"]		= [sm = stateManager]() {
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::MenuStats);
	};

	myCallbacks["Credits"]		= [sm = stateManager]() {
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::MenuCredits); 
	};

	myCallbacks["Pause"]		= [sm = stateManager]() {
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PushState(ISTE::StateType::MenuPause);
	};

	myCallbacks["Return"]		= [sm = stateManager]() { 
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick);
		sm->PopState(); 	
	};
	
	// Load Levels
	myCallbacks["Load_Level_0"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 0);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Load_Level_1.1"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 1);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Load_Level_1.2"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 2);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Load_Level_2.1"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 3);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Load_Level_2.2"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 4);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};
	myCallbacks["Load_Level_3"] = [db = genericDatabase, sm = stateManager]() {
		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 5);
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_StartGame);
		sm->PushState(ISTE::StateType::FadeOut);
	};

	// Volume
	myCallbacks["Volume_Master_Higer"]	 = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eMaster, 0.1f); 		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_Master_Lower"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eMaster, -0.1f); 		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_SFX_Higer"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eSFX, 0.1f); 				Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_SFX_Lower"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eSFX, -0.1f); 			Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_Music_Higer"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eMusic, 0.1f); 			Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_Music_Lower"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eMusic, -0.1f); 		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_Ambience_Higer"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eAmbiance, 0.1f); 	Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_Ambience_Lower"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eAmbiance, -0.1f); 	Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_UI_Higer"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eUI, 0.1f); 				Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Volume_UI_Lower"] = [ah = audioHandler]() { ah->IncrementGlobalVolume(ISTE::SoundTypes::eUI, -0.1f); 				Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };

	// Resolution, these will crash if imgui exists
	myCallbacks["Toggle_Fullscreen"] = [wnd = Context::Get()->myWindow]() { wnd->ToggleFullscreen(); 								Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Set_Resolution_720"] = [wnd = Context::Get()->myWindow]() { wnd->SetResolution(CU::Vec2Ui(1280, 720)); 			Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Set_Resolution_900"] = [wnd = Context::Get()->myWindow]() { wnd->SetResolution(CU::Vec2Ui(1600, 900)); 			Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Set_Resolution_1080"] = [wnd = Context::Get()->myWindow]() { wnd->SetResolution(CU::Vec2Ui(1920, 1080)); 			Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };
	myCallbacks["Set_Resolution_1440"] = [wnd = Context::Get()->myWindow]() { wnd->SetResolution(CU::Vec2Ui(2560, 1440)); 			Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eUI_ButtonClick); };

}

void ISTE::UIHandler::InitMouse()
{
	myMouse.myTextureId[ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/UI/UI_mouseCursor_64x64.dds").myValue;
	myMouse.myScale = CU::Vec2f(0.1f / ASPECT_RATIO, 0.1f);
	myMouse.myPivot = CU::Vec2f(-0.5f, 0.5f);
	myMouseEmitter = myCtx->mySprite2DParticleHandler->SpawnEmitter("MouseStatic");
}
