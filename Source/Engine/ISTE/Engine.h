#pragma once
#include <Windows.h> 

//ENGINES
#include "Context.h"
#include "Graphics/DX11.h"
#include "Graphics/GraphicsEngine.h"

//MANAGERS
#include "Scene/SceneHandler.h"
#include "ECSB/SystemManager.h"
#include "Graphics/Resources/ShaderManager.h"
#include "Graphics/Resources/ModelManager.h"
#include "Graphics/Resources/AnimationManager.h"
#include "Graphics/Resources/TextureManager.h"
#include "Graphics/RenderStateManager.h"
#include "ISTE/Text/TextService.h"
#include "ISTE/Audio/AudioHandler.h"

//CU and misc
#include "ISTE/CU/InputHandler.h"
#include "ISTE/CU/Timer.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerPool.h"
#include "ISTE/Math/Vec2.h"
#include "ISTE/EngineRegister.h"
#include "ISTE/GameRegister.h"
#include "WindowsWindow.h"
#include "ISTE/Events/EventHandler.h"
#include "ISTE/UI/UIHandler.h"
#include "ISTE/CU/Database.h"
#include "ISTE/StatsFondler.h"

#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateStack.h"

#include "ISTE/VFX/VFXHandler.h"
#include "ISTE/VFX/VFXSystemDefines.h"
#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXDefines.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleDefines.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleDefines.h"

#include "../Editor/BaseEditor.h"
#include "ISTE/Physics/PhysicsEngine.h"

namespace ISTE {

	enum class EngineState {
		eRunning,
		eShuttingDown,
		eCount,

	};


	class Engine
	{
	public:
		Engine();
		~Engine();

		bool InternalStartup(const CU::Vec2Ui& aRes/*, callback_function_wndProc& aWndProcCallback*/);

		HWND* GetWindowsHandle() { return &myWindowsHandle; }
		EngineState& GetEngineState() { return myEngineState; }

		void Update(const float aDeltaTime);
		void Render();
	
	private:
		EngineState			myEngineState;
		HWND				myWindowsHandle;

		//Engines
		Context				myContext;
		DX11				myDX11;
		GraphicsEngine		myGraphicsEngine;
		PhysicsEngine		myPhysicsEngine;

		//Managers
		SceneHandler		mySceneHandler;
		SystemManager		mySystemManager;
		ShaderManager		myShaderManager;
		ModelManager		myModelManager;
		AnimationManager	myAnimationManager;
		TextureManager		myTextureManager;
		RenderStateManager	myRenderStateManager; 
		AudioHandler		myAudioHandler;

		WindowsWindow		myWindow;

		//CU & misc
		CU::InputHandler	myInputHandler;
		CU::Timer			myCUTimer;
		TimeHandler			myTimeHandler;
		TimerPool			myTimerPool;
		EventHandler		myEventHandler;
		UIHandler			myUIHandler;
		CU::Database<true>	myGenericDatabase;
		StatsFondler		myStatsFondler;

		// StateStack
		StateStackTable	    myStateStackTable;
		StateStack			myStateStack;
		StateManager		myStateManager;

		// VFX
		VFXHandler				myVFXHandler;
		ModelVFXHandler			myModelVFXHandler;
		Sprite2DParticleHandler	mySprite2DParticleHandler;
		Sprite3DParticleHandler	mySprite3DParticleHandler;
		VFXSystem			myVFXSystem;
		ModelVFXSystem		myModelVFXSystem;
		Emitter2DSystem		myEmitter2DSystem;
		Emitter3DSystem		myEmitter3DSystem;

		EngineRegister		myEngineRegister;
		GameRegister		myGameRegister;
	};


};