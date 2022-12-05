#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//#define OEMRESOURCE

#include <Windows.h>
#include "ISTE/Engine.h"
#include "ISTE/Context.h"
#include "BaseEditor.h"

#include <Windows.h>
#include "ISTE/Logger/Logger.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Physics/PhysicsEngine.h"

#include <d3d11.h>
#include "DirectXTex/DDSTextureLoader/DDSTextureLoader11.h"

//Components
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite3DComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Text/TextComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransperancyComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "ISTE/VFX/SpriteParticles/EmitterSystem.h"

#include "ISTE/ComponentsAndSystems/LoDSystem.h"

//used for testing
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/ECSB/ComponentPool.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/ECSB/PlayerBehaviour.h"

#include <random>


#include "ISTE/CU/Database.h"
#include "ISTE/CU/MemoryTracker.h"

#include "ISTE/CU/MemTrack.hpp"


#include "ISTE/Audio/AudioSource.h"
#include "ISTE/Audio/EventAudioSource.h"


#pragma region WindowsInit
void InitConsole()
{
#pragma warning( push )
#pragma warning( disable : 4996 )
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
#pragma warning( pop )
}

void CloseConsole()
{
#pragma warning( push )
#pragma warning( disable : 4996 )
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
#pragma warning( pop )
}

#pragma endregion

#pragma comment(lib, "winmm.lib")


#include "ISTE/CU/Helpers.h"

#define TestCursor 1000

int Scope(HINSTANCE&, int)
{
	unsigned int width = 1600;
	unsigned int height = 900;

	ISTE::Engine engine;
	ISTE::Context* ctx = ISTE::Context::Get();

	if (!engine.InternalStartup({ width, height }))
	{
		//ERROR - failed init
		return -1;
	}
	ctx->myDX11->SetVoidColor({ 204.f / 255.f, 102 / 255.f, 102 / 255.f, 1 });

	ISTE::BaseEditor editor;
	editor.Init();

	//testing grounds
	ctx->mySceneHandler->LoadScene(0);
	ISTE::Scene& scene = ctx->mySceneHandler->GetActiveScene();


	ctx->myWindow->SetResolution(CU::Vec2Ui(width, height));
	ISTE::BaseEditor::myWantToUpdateSize = true;
	ctx->myGraphicsEngine->SetCamera(ctx->mySceneHandler->GetCamera());

	ctx->myAudioHandler->HookListener(ctx->mySceneHandler->GetCamera().GetTransform());

	//line example
	float timeElepsed = 0;
	float delta	= 0;
	ctx->myGraphicsEngine->GetDebugDrawer().AddStaticLineCommand(
		ISTE::LineCommand{
		{0,0,0},
		{0,5,0},
		{1,0,0} }
	);

	// Set cursor
	{
		//HWND wHandle;
		//wHandle = ctx->myWindow->GetWindowsHandle();
		////HCURSOR Cursor = LoadCursor(NULL, IDC_HAND); // IDC_ARROW IDC_WAIT IDC_HAND...  http://msdn.microsoft.com/en-us/library/ms648391%28v=vs.85%29.aspx
		//HCURSOR Cursor = LoadCursorFromFile(L"../Assets/Sprites/Ui/Cursors/UI_mouseCursor_64x64.cur"); //.cur or .ani
		//SetCursor(Cursor);
		//SetClassLongPtr(wHandle, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(Cursor));
	}

	//test
	//ctx->myAudioHandler->Play("../Assets/Audio/Music/CFAR.wav");

	// push start up states
	ctx->myStateManager->LoadStack("Splash Screen");
	//ctx->myStateManager->PushState(ISTE::StateType::HUD);
	//ctx->myStateManager->PushState(ISTE::StateType::FadeIn);
	//ctx->myStateManager->PushState(ISTE::StateType::SplashScreen); 
	 
	//gs test thing, dont remove 
	//EntityID ent	= scene.NewEntity();
	//auto* tra		= scene.AssignComponent<ISTE::TransformComponent>(ent);
	//auto* mats		= scene.AssignComponent<ISTE::MaterialComponent>(ent);
	//mats->myTextures[0][0] = ctx->myTextureManager->LoadTexture(L"ad", true);
	//mats->myTextures[0][1] = ctx->myTextureManager->LoadTexture(L"ad", false);
	//mats->myTextures[0][2] = ctx->myTextureManager->LoadTexture(L"ad", false);
	//auto& mod		= ctx->myModelManager->LoadModel(ent,"../Assets/Models/Characters/CH_PL_Player_RIG.fbx");
	//auto& anim		= ctx->myAnimationManager->LoadAnimation(ent,"../Assets/Animations/CH_Player_idle_ANIM.fbx"); 
	//auto* bleeeeh	= scene.AssignComponent<ISTE::AnimationBlendComponent>(ent);
	//tra->myPosition = { 0,1,0 };
	//
	//
	//bleeeeh->myAnimationBlendData.myRootNode.myDataIndex = 1;
	//bleeeeh->myAnimationBlendData.myRootNode.myType = ISTE::AnimBlendOperationType::eInterpolate;
	//bleeeeh->myAnimationBlendData.myInterpOperations[0].myLerpValue = 0.5f; 
	//bleeeeh->myAnimationBlendData.myFetchOperations[0].myAnimation = ctx->myAnimationManager->LoadAnimation(mod.myValue, "../Assets/Animations/CH_Player_meleeAttack_ANIM.fbx");
	//bleeeeh->myAnimationBlendData.myFetchOperations[1].myAnimation = ctx->myAnimationManager->LoadAnimation(mod.myValue, "../Assets/Animations/CH_Player_Spell_ANIM.fbx");
	//bleeeeh->myAnimationBlendData.myFetchOperations[2].myAnimation = ctx->myAnimationManager->LoadAnimation(mod.myValue,"../Assets/Animations/CH_Player_ThrowAbility_ANIM.fbx");
	//
	//bleeeeh->myAnimationBlendData.myInterpOperations[0].myNodes[0].myDataIndex = 0;
	//bleeeeh->myAnimationBlendData.myInterpOperations[0].myNodes[0].myType = ISTE::AnimBlendOperationType::eFetchAnimation;
	//
	//bleeeeh->myAnimationBlendData.myInterpOperations[0].myNodes[1].myDataIndex = 1;
	//bleeeeh->myAnimationBlendData.myInterpOperations[0].myNodes[1].myType = ISTE::AnimBlendOperationType::eFetchAnimation;
	//
	//bleeeeh->myAnimationBlendData.myInterpOperations[1].myLerpValue = 0.5f;
	//bleeeeh->myAnimationBlendData.myInterpOperations[1].myNodes[0].myType = ISTE::AnimBlendOperationType::eInterpolate;
	//bleeeeh->myAnimationBlendData.myInterpOperations[1].myNodes[0].myDataIndex = 0;
	//bleeeeh->myAnimationBlendData.myInterpOperations[1].myNodes[1].myType = ISTE::AnimBlendOperationType::eFetchAnimation;
	//bleeeeh->myAnimationBlendData.myInterpOperations[1].myNodes[1].myDataIndex = 2;

	MSG msg			= {};
	bool shouldRun = true;
	while (engine.GetEngineState() != ISTE::EngineState::eShuttingDown)
	{
		//bleeeeh->myAnimationBlendData.myInterpOperations[0].myLerpValue = (sin(ctx->myTimeHandler->GetTotalTime() * 10) * 0.5) + 0.5;
		//bleeeeh->myAnimationBlendData.myInterpOperations[1].myLerpValue = (sin(ctx->myTimeHandler->GetTotalTime()) * 0.5) + 0.5;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				engine.GetEngineState() = ISTE::EngineState::eShuttingDown;
			}
		}
		//BEGIN
		ctx->myDX11->BeginFrame();
		ctx->myTimeHandler->TickTimers();
		ctx->myTimeHandler->ProcessTimers();
		if (engine.GetEngineState() != ISTE::EngineState::eShuttingDown)
		{
			editor.StartFrame();//DoWholeImGui();
		}

		//Update
		//-
			//line example
		timeElepsed	= ctx->myTimeHandler->GetTotalTime();
		delta			= ctx->myTimeHandler->GetDeltaTime();
		ctx->myGraphicsEngine->GetDebugDrawer().AddDynamicLineCommand(
			ISTE::LineCommand{
			{cos(timeElepsed)		 ,1,sin(timeElepsed)},
			{cos(timeElepsed - (delta * 2)),1,sin(timeElepsed - (delta * 2))},
			{0,0,1} }
		);
		if (engine.GetEngineState() != ISTE::EngineState::eShuttingDown)
		{
			editor.Update();
		}
		ctx->myAudioHandler->Update();
		//ctx->mySceneHandler->Update(ctx->myTimer->GetDeltaTime());
		
		engine.Update(delta);

		ctx->mySystemManager->GetSystem<ISTE::LoDSystem>()->UpdateAgainstPlayer();

		//Rendering
		ctx->myGraphicsEngine->PrepareRenderCommands();  
		editor.PrepareIconRenderCommands(); 
		engine.Render();
		ctx->myGraphicsEngine->Render();
		editor.DrawGizmos();
		ctx->myGraphicsEngine->ClearCommands();
		ctx->myGraphicsEngine->SelectRenderPass();
		ctx->myGraphicsEngine->CopyToBackBuffer();
		

		//END
		if (engine.GetEngineState() != ISTE::EngineState::eShuttingDown)
		{
			editor.EndFrame();
		}
		ctx->myDX11->EndFrame();
		ctx->myInputHandler->Refresh();
		ISTE::Logger::FlushDrawCall();
	}

	return (int)msg.wParam;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,
	_In_ int nCmdShow
)
{

	InitConsole();

	CU::MemoryTracker tracker;

	tracker.StartTracking(false);

	int scope = Scope(hInstance, nCmdShow);
		
	tracker.StopAndPrintAllocationData();

	CloseConsole();

	return scope;
}