#pragma once

namespace ISTE
{
	struct Context;
	class SceneHandler;
	class SystemManager;
	class EngineRegister
	{
	public:
		void Init();
		void RegisterComponents();
		void RegisterBehaviours();
		void RegisterSystems();
		void RegisterObjectBuilders();
		void RegisterReadAndWrites();

	private:
		Context* myCtx;
		SceneHandler* mySceneHandler;
		SystemManager* mySystemManager;
	};
}