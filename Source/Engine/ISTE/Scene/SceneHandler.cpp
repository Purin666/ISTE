#include "SceneHandler.h"
#include "Scene.h"

#include "../Builder/SceneBuilder.h"
#include "../ComponentsAndSystems/BehaviourSystem.h"

#include "../Context.h"
#include "../Graphics/DX11.h"
#include "../WindowsWindow.h"

#include "ISTE/CU/MemTrack.hpp"

//temp
#include "ISTE/Events/EventHandler.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/ComponentAndSystem/CullingSystem.h"
//

namespace ISTE
{
	SceneHandler::~SceneHandler()
	{
		if (myActiveScene)
			delete myActiveScene;

		delete myBuilder;
	}
	SceneHandler::SceneHandler()
	{

	}
	void SceneHandler::Init()
	{
		myBuilder = new SceneBuilder();
		myBuilder->mySceneHandler = this;
		myBuilder->Init();
		myBehaviourSystem = Context::Get()->mySystemManager->GetSystem<BehaviourSystem>();
		myBehaviourSystem->mySceneHandler = this;

		myCamera.SetPerspectiveProjection(90, Context::Get()->myWindow->GetResolution(), 0.5, 5000);

		Context::Get()->mySystemManager->GetSystem<CullingSystem>()->Init();
	}
	void SceneHandler::Update(float aTimeDelta)
	{
		myActiveScene->PostUpdateCalls(); // this was moved from the end of func

		if (myWantToLoadBuildScene)
		{
			if (myActiveScene)
			{
				delete myActiveScene;
				myActiveScene = nullptr;
				Context::Get()->myEventHandler->InvokeEvent(EventType::SceneDestroyed, 0);
			}

			myBuilder->LoadScene(myBuildScene);
			AddPPEffectsToDatabase();

			//temp
			Context::Get()->myGraphicsEngine->SetCamera(myCamera);
			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->SceneLoaded();

			myBehaviourSystem->Init();
			myWantToLoadBuildScene = false;
		}

		if (myWantToLoadTestScene)
		{
			if (myActiveScene)
			{
				delete myActiveScene;
				myActiveScene = nullptr; 
				Context::Get()->myEventHandler->InvokeEvent(EventType::SceneDestroyed, 0);
			}

			myBuilder->LoadScene(myTestScene);
			AddPPEffectsToDatabase();

			Context::Get()->myGraphicsEngine->SetCamera(myCamera);
			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->SceneLoaded();

			myBehaviourSystem->Init();
			myWantToLoadTestScene = false;
		}

		myBehaviourSystem->Update(aTimeDelta);
		//Context::Get()->mySystemManager->GetSystem<CullingSystem>()->Cull();

		//myActiveScene->PostUpdateCalls(); // this was moved to top of func
	}
	void SceneHandler::PostLogicUpdate()
	{
		if(myActiveScene)
			myActiveScene->PostUpdateCalls();
	}
	void SceneHandler::LoadScene(size_t aIndex, bool aForce)
	{
		if (aForce)
		{
			if (myActiveScene)
			{  
				delete myActiveScene;
				myActiveScene = nullptr;
				Context::Get()->myEventHandler->InvokeEvent(EventType::SceneDestroyed, 0);
			}

			Context::Get()->myGraphicsEngine->SetCamera(myCamera);

			myBuilder->LoadScene(aIndex);

			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->SceneLoaded();

			myBehaviourSystem->Init();
		}
		else if (myActiveScene == nullptr)
		{
			myBuilder->LoadScene(aIndex);

			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->SceneLoaded();

			myBehaviourSystem->Init();
		}
		else
		{
			myWantToLoadBuildScene = true;
		}
		myBuildScene = aIndex;
		AddPPEffectsToDatabase();// temp //mathias
	}
	void SceneHandler::LoadScene(std::string aIndex, bool aForce)
	{
		if (aForce)
		{
			if (myActiveScene)
			{
				delete myActiveScene;
				myActiveScene = nullptr;
				Context::Get()->myEventHandler->InvokeEvent(EventType::SceneDestroyed, 0);
			}
			myBuilder->LoadScene(aIndex);

			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->SceneLoaded();

			myBehaviourSystem->Init();
		}
		else if (myActiveScene == nullptr)
		{
			myBuilder->LoadScene(aIndex);

			Context::Get()->mySystemManager->GetSystem<CullingSystem>()->SceneLoaded();

			myBehaviourSystem->Init();
		}
		else
		{
			myWantToLoadTestScene = true;
		}

		myTestScene = aIndex;
		AddPPEffectsToDatabase();// temp //mathias
	}
	void SceneHandler::CreateEmptyScene()
	{
		{
			if (myActiveScene)
			{
				delete myActiveScene;
				Context::Get()->myEventHandler->InvokeEvent(EventType::SceneDestroyed, 0);
			}

			myActiveScene = new Scene;
			myActiveScene->mySceneHandler = this;
			AddPPEffectsToDatabase(); // temp //mathias
		}
	}
	void SceneHandler::AddPPEffectsToDatabase()// temp //mathias
	{
		CU::Database<true>& db = myActiveScene->GetDatabase();

		//db.SetValue("BloomBlending", .3f);
		//db.SetValue("Saturation", 1.0f);
		//db.SetValue("Exposure", 1.0f);
		//db.SetValue("Contrast", CU::Vec3f(1.f, 1.f, 1.f));
		//db.SetValue("Tint", CU::Vec3f(1.f, 1.f, 1.f));
		//db.SetValue("Blackpoint", CU::Vec3f(0.f, 0.f, 0.f));
	}
	int SceneHandler::GetActiveSceneIndex()
	{
		return myActiveScene->myId;
	}
	int SceneHandler::GetTotalBuildScenes()
	{
		return (int)myBuilder->myBuildScenes.size();
	}
}