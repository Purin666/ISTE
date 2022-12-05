#pragma once

class StatsFondler;
namespace CU
{
	class InputHandler;
	template <bool>
	class Database;
}
namespace ISTE
{
	class DX11;
	class GraphicsEngine;

	class SceneHandler;
	class SystemManager;
	class ShaderManager;
	class ModelManager;
	class AnimationManager;
	class TextureManager;
	class RenderStateManager;
	class Engine;
	class WindowsWindow;
	//class TextService;
	class PhysicsEngine;
	class TimeHandler;
	class EventHandler;
	class UIHandler;
	class AudioHandler;
	class VFXHandler;
	class ModelVFXHandler;
	class Sprite2DParticleHandler;
	class Sprite3DParticleHandler;
	class StateManager;

	class Logger;

	struct Context
	{
	public:
		inline static Context* Get() { return myInstance; }
		//Engines
		Engine*				myEngine;
		DX11*				myDX11;
		GraphicsEngine*		myGraphicsEngine;
		PhysicsEngine*		myPhysicsEngine;
		
		//Managers
		SceneHandler*		mySceneHandler;
		SystemManager*		mySystemManager;
		ModelManager*		myModelManager;
		AnimationManager*	myAnimationManager;
		ShaderManager*		myShaderManager;
		TextureManager*		myTextureManager;
		RenderStateManager* myRenderStateManager;
		//TextService*		myTextService;  //<< should be called TextManager 
		AudioHandler*		myAudioHandler;
		StateManager*		myStateManager;

		//CU and misc
		CU::InputHandler*			myInputHandler;
		TimeHandler*				myTimeHandler; 
		WindowsWindow*				myWindow;
		EventHandler*				myEventHandler;
		UIHandler*					myUIHandler;
		StatsFondler*				myStatsFondler;

		VFXHandler*					myVFXHandler;
		ModelVFXHandler*			myModelVFXHandler;
		Sprite2DParticleHandler*	mySprite2DParticleHandler;
		Sprite3DParticleHandler*	mySprite3DParticleHandler;

		CU::Database<true>*			myGenericDatabase;

	private:
		friend class Engine;
		inline static Context* myInstance;


	};
};