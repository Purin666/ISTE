#include "Engine.h" 

#include "ISTE/Helper/JsonDefines.h"
#include "ISTE/Helper/JsonIO.h"

#include "ISTE/VFX/SpriteParticles/EmitterSystem.h" // included to init

//#include "StateType.h"

namespace ISTE
{
	void RegisterStates(StateStack&); // Foward declaration; implemented in RegisterStates.cpp
}


ISTE::Engine::Engine()
{
	//Engines
	myContext.myInstance				= &myContext;
	myContext.myEngine					= this;
	myContext.myDX11					= &myDX11;
	myContext.myGraphicsEngine			= &myGraphicsEngine;
	myContext.myPhysicsEngine			= &myPhysicsEngine;

	//Managers
	myContext.mySceneHandler			= &mySceneHandler;
	myContext.mySystemManager			= &mySystemManager;
	myContext.myModelManager			= &myModelManager;
	myContext.myAnimationManager		= &myAnimationManager;
	myContext.myShaderManager			= &myShaderManager;
	myContext.myTextureManager			= &myTextureManager;
	myContext.myRenderStateManager		= &myRenderStateManager; 
	myContext.myAudioHandler			= &myAudioHandler;
	myContext.myStateManager			= &myStateManager;
	myStateManager.myStack				= &myStateStack;

	//CU & misc
	myContext.myInputHandler			= &myInputHandler;
	myContext.myTimeHandler				= &myTimeHandler;
	myContext.myWindow					= &myWindow;
	myContext.myEventHandler			= &myEventHandler;
	myContext.myUIHandler				= &myUIHandler;
	myContext.myGenericDatabase			= &myGenericDatabase;
	myContext.myStatsFondler			= &myStatsFondler;

	// VFX
	myContext.myVFXHandler				= &myVFXHandler;
	myVFXHandler.mySystem				= &myVFXSystem;
	myContext.myModelVFXHandler			= &myModelVFXHandler;
	myModelVFXHandler.mySystem			= &myModelVFXSystem;
	myContext.mySprite2DParticleHandler = &mySprite2DParticleHandler;
	mySprite2DParticleHandler.mySystem	= &myEmitter2DSystem;
	myContext.mySprite3DParticleHandler = &mySprite3DParticleHandler;
	mySprite3DParticleHandler.mySystem	= &myEmitter3DSystem;

	// time
	myTimeHandler.myPool				= &myTimerPool;
	myTimeHandler.myCUTimer				= &myCUTimer;
}

ISTE::Engine::~Engine()
{
}

bool ISTE::Engine::InternalStartup(const CU::Vec2Ui& aRes)
{
	if (!myWindow.Init(aRes))
	{
		MessageBox(nullptr, L"Tell a programmer", L"WINDOW FAILED TO INIT", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!myDX11.Init(myWindow.GetWindowsHandle()))
	{
		MessageBox(nullptr, L"Tell a programmer", L"DX11 FAILED TO INIT", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!myRenderStateManager.Init())
	{
		MessageBox(nullptr, L"Tell a programmer", L"RENDER STATE MANAGER FAILED TO INIT", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!myModelManager.Init())
	{
		MessageBox(nullptr, L"Tell a programmer", L"MODEL MANAGER FAILED TO INIT", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!myTextureManager.Init())
	{
		MessageBox(nullptr, L"Tell a programmer", L"TEXTURE MANAGER FAILED TO INIT", MB_OK | MB_ICONERROR);
		return false;
	}
	 
	if (!myShaderManager.Init())
	{
		MessageBox(nullptr, L"Tell a programmer", L"SHADER MANAGER FAILED TO INIT", MB_OK | MB_ICONERROR);
		return false;
	}

	//REgister
	myAnimationManager.Init();
	myEngineRegister.Init();
	myGameRegister.Init();

	myEngineRegister.RegisterComponents();
	myEngineRegister.RegisterBehaviours();
	myGameRegister.RegisterComponents();
	myGameRegister.RegisterBehaviours();

	myEngineRegister.RegisterSystems();
	myGameRegister.RegisterSystems();

	myPhysicsEngine.Init();
	mySceneHandler.Init(); 
	myEngineRegister.RegisterObjectBuilders();
	myEngineRegister.RegisterReadAndWrites();

	//

	if (!myGraphicsEngine.Init())
		return false;

	myWindowsHandle = myWindow.GetWindowsHandle();


	// Init StateStack
	InitStack(myStateStack);
	RegisterStates(myStateStack);

	// Init, Load VFX and Particles
	{
		LoadJson("../Assets/VFX/Data/ModelVFXTypes.json", myModelVFXSystem.myDatas);
		myModelVFXHandler.LoadModelsAndTextures();

		LoadJson("../Assets/VFX/Data/Emitter2DTypes.json", myEmitter2DSystem.myEmitterDatas);
		mySprite2DParticleHandler.LoadParticleTextures();

		LoadJson("../Assets/VFX/Data/Emitter3DTypes.json", myEmitter3DSystem.myEmitterDatas);
		mySprite3DParticleHandler.LoadParticleTextures();
		myContext.mySystemManager->GetSystem<EmitterSystem>()->Init();

		LoadJson("../Assets/VFX/Data/VFXData.json", myVFXSystem.myDatas);
		//myVFXHandler.LoadModelsAndTextures();
	}

	// Load UI
	LoadJson("../Assets/MiscData/UIData.json", myUIHandler.myUIDatas);
	myUIHandler.Init();
	
	// Load StateStacks
	LoadJson("../Assets/MiscData/StateStacks.json", myStateStackTable);

	myStatsFondler.Init();

	myEngineState = EngineState::eRunning;
	return true;
}

void ISTE::Engine::Update(const float aDeltaTime)
{
	// states
	{
		while (myStateManager.myPopCount > 0)
		{
			PopState(myStateStack);
			--myStateManager.myPopCount;
		}

		if (!myStateManager.myStackToLoad.empty())
		{
			LoadStack(myStateStack, myStateStackTable, myStateManager.myStackToLoad);
			myStateManager.myStackToLoad.clear();
		}

		UpdateStates(myStateStack, aDeltaTime);
	}
	// vfx
	mySystemManager.GetSystem<EmitterSystem>()->UpdateEmitterPositions();
	myVFXHandler.Update(aDeltaTime);
	myModelVFXHandler.Update(aDeltaTime);
	mySprite2DParticleHandler.UpdateEmittersAndParticles(aDeltaTime);
	mySprite3DParticleHandler.UpdateEmittersAndParticles(aDeltaTime);
	// ui callbacks
	{
		auto& cbs = myUIHandler.myCallbacksToExecute;
		while (!cbs.empty())
		{
			if (cbs.front() != nullptr)
			{
				cbs.front()();
			}
			cbs.pop();
		}
	}
	// update custom mouse
	myUIHandler.UpdateMouse();
}
void ISTE::Engine::Render()
{
	// states
	RenderStates(myStateStack);

	// vfx
	mySprite2DParticleHandler.CreateRenderCommands();
	mySprite3DParticleHandler.CreateRenderCommands();

	// render custom mouse
	myUIHandler.RenderMouse();
}

