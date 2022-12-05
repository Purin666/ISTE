#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include "ISTE/Engine.h"
#include "ISTE/Context.h"

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

	//testing grounds
	ctx->mySceneHandler->LoadScene(0);
	ISTE::Scene& scene = ctx->mySceneHandler->GetActiveScene();

#ifdef _DEBUG
	ctx->myWindow->SetResolution({ width, height });
#else
	ctx->myWindow->SetFullscreen(true);
#endif // _DEBUG

	ctx->myGraphicsEngine->SetCamera(ctx->mySceneHandler->GetCamera());

	ctx->myAudioHandler->HookListener(ctx->mySceneHandler->GetCamera().GetTransform());

	//line example
	float timeElepsed = 0;
	float delta = 0;

	ctx->myInputHandler->SetCursorVisibility(false);
	// push start up states
	ctx->myStateManager->LoadStack("Splash Screen");


	MSG msg = {};
	bool shouldRun = true;
	while (engine.GetEngineState() != ISTE::EngineState::eShuttingDown)
	{
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

		//Update
		delta = ctx->myTimeHandler->GetDeltaTime();
		ctx->myAudioHandler->Update();

		engine.Update(delta);

		ctx->mySystemManager->GetSystem<ISTE::LoDSystem>()->UpdateAgainstPlayer();


		//Rendering
		ctx->myGraphicsEngine->PrepareRenderCommands();
		engine.Render();
		ctx->myGraphicsEngine->Render();
		ctx->myGraphicsEngine->ClearCommands();
		ctx->myGraphicsEngine->CopyToBackBuffer();


		//END
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

#ifdef _DEBUG
	InitConsole();
#endif
	int scope = Scope(hInstance, nCmdShow);
#ifdef _DEBUG
	CloseConsole();
#endif
	return scope;
}