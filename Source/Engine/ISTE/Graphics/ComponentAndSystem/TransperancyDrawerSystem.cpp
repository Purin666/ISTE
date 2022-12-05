#include "TransperancyDrawerSystem.h"
#include <d3d11.h>
#include <fstream>

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>

#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite3DComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/TransperancyComponent.h>

#include <ISTE/Graphics/GraphicsEngine.h>
#include <ISTE/Graphics/Resources/ModelManager.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ShaderManager.h>
#include <ISTE/Graphics/Shaders/cpp/Shader.h>


void ISTE::TransperancyDrawerSystem::Init()
{
	myCtx = Context::Get();
}

void ISTE::TransperancyDrawerSystem::PrepareRenderCommands()
{
	//change to camera, or camera component, or whatever is used later on
	//EditorCamera* myCamera = myCtx->myGraphicsEngine->GetCamera();
	SceneHandler* sceneHandler = myCtx->mySceneHandler;
	Scene& scene = sceneHandler->GetActiveScene();
	
	
	for (EntityID entity : myEntities[0])
	{
		//TransformComponent* transformData = scene.GetComponent<TransformComponent>(entity);
		TransperancyComponent* transperancyComponent = scene.GetComponent<TransperancyComponent>(entity);
		RenderCommand rc = {
			rc.myEntity = entity, 
			rc.myShader = transperancyComponent->myShader,
			rc.myDistToCamera = 0.f//(myCamera->GetPosition() - transformData->myPosition).Length()
		};
		myRenderCommands.Enqueue(rc);
	}
}

void ISTE::TransperancyDrawerSystem::Draw()
{
	//sShaderManager* sm = myCtx->myShaderManager;
	//sauto commands = myRenderCommands;
	//ssize_t size = myRenderCommands.GetSize();
	//sfor (; size != 0; size--)
	//s{
	//s	RenderCommand rc = commands.Dequeue();
	//s	sm->BindShader(rc.myShader);
	//s	sm->Draw(rc.myShader, rc.myEntity);
	//s}
}

void ISTE::TransperancyDrawerSystem::ClearCommands()
{
	myRenderCommands.Clear();
}
