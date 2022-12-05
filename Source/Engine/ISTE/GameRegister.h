#pragma once

namespace ISTE
{
	struct Context;
	class SceneHandler;
	class SystemManager;
	class GameRegister
	{
	public:
		void Init();
		void RegisterComponents();
		void RegisterBehaviours();
		void RegisterSystems();

	private:
		Context* myCtx;
		SceneHandler* mySceneHandler;
		SystemManager* mySystemManager;
	};
}