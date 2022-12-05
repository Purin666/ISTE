#include "LightDrawerSystem.h"
#include "ISTE/Context.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/RenderStateManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Graphics/Resources/ShaderManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/ECSB/ECSDefines.hpp"

#include "ISTE/Graphics/ComponentAndSystem/CullingSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/PointLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/SpotLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AmbientLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ShadowCastingPointLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ShadowCastingSpotLightComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/BillboardRenderCommand.h"



//for culling
#include "ISTE/Events/EventHandler.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"

#include "ISTE/CU/MemTrack.hpp"

struct PointLightBuffer
{
	CU::Matrix4x4f myTransform;
	CU::Vec4f myColorAndIntensity;
	float myRange;
	CU::Vec2f myLightPerspectiveValues;
	float Garb;

};

struct SpotLightBuffer
{
	CU::Matrix4x4f myLightProjection;
	CU::Matrix4x4f myTransform;
	CU::Vec4f myColorAndIntensity;
	CU::Vec3f myDirection;
	float garb1;

	float myRange; 
	float myInnerLimit; 
	float myOuterLimit; 
	float garb2;
};

bool ISTE::LightDrawerSystem::Init()
{
	myCtx = Context::Get();
	myLightIcons[(int)LightComplexity::ePointLight]			= myCtx->myTextureManager->LoadTexture(L"../EngineAssets/Images/LightIcons/PointLightIcon.dds", true);
	myLightIcons[(int)LightComplexity::eSpotLight]			= myCtx->myTextureManager->LoadTexture(L"../EngineAssets/Images/LightIcons/SpotLightIcon.dds", true);
	myLightIcons[(int)LightComplexity::eDirectionalLight]	= myCtx->myTextureManager->LoadTexture(L"../EngineAssets/Images/LightIcons/DirectionalLightIcon.dds", true);
	myLightIcons[(int)LightComplexity::eAmbientLight]		= myCtx->myTextureManager->LoadTexture(L"../EngineAssets/Images/LightIcons/AmbientlLightIcon.dds", true);
	myPLightModel = myCtx->myModelManager->LoadModel("../Assets/Models/Ico_Sphere.fbx");

	if (!InitShader("Shaders/PointLight_VS.cso", "Shaders/PointLight_PS.cso",LightComplexity::ePointLight))
		return false;
	if (!InitShader("Shaders/PointLight_VS.cso", "Shaders/ShadowPointLight_PS.cso", LightComplexity::eShadowCastingPointLight))
		return false;

	if (!InitShader("Shaders/SpotLight_VS.cso", "Shaders/SpotLight_PS.cso", LightComplexity::eSpotLight))
		return false;
	if (!InitShader("Shaders/SpotLight_VS.cso", "Shaders/ShadowSpotLight_PS.cso", LightComplexity::eShadowCastingSpotLight))
		return false;

	if(!InitPointLightBuffer())
		return false;
	if (!InitSpotLightBuffer())
		return false;

	InitPointLightMatrixList();

	//myLightDepthTarget.Create(myCtx->myDX11->GetBackBufferRes());
	myLightDepthTarget.Create({ 2000,2000 });
	myLightCubeDepthTarget.Create({ 250,250 }); 

	Context::Get()->myEventHandler->RegisterCallback(EventType::EntityMoved, "LightSystem_EntityMoved", [this](EntityID aId) {UpdateShadowCastersEntityList(aId); });
	Context::Get()->myEventHandler->RegisterCallback(EventType::EntityActivated, "LightSystem_EntityActivated", [this](EntityID aId) {UpdateShadowCastersEntityList(aId); });
	
	Context::Get()->myEventHandler->RegisterCallback(EventType::EntityDeactivated, "LightSystem_EntityDeactivated", [this](EntityID aId) {RemoveFromShadowCasterEntityList(aId); });
	Context::Get()->myEventHandler->RegisterCallback(EventType::EntityDestroyed, "LightSystem_EntityDestroyed", [this](EntityID aId) {RemoveFromShadowCasterEntityList(aId); });

	return true;
}

void ISTE::LightDrawerSystem::PrepareLightCommands()
{
	PrepareSpotLights();
	PreparePointLights();
	PrepareShadowPointLights();
	PrepareShadowSpotLights();
	PrepareDirectionalLights();
	PrepareAmbientLights(); 
}

void ISTE::LightDrawerSystem::PrepareLightIcons()
{
	SpriteDrawerSystem* sds = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	DebugDrawer* dd = &myCtx->myGraphicsEngine->GetDebugDrawer();

	BillboardCommand bRC; 
	LineCommand lC;
	lC.myColor = { 1,0,0 };

	bRC.myTransform.GetRow(1) *= 0.1f;
	bRC.myTransform.GetRow(2) *= 0.1f;
	


	bRC.myTextures = myLightIcons[(int)LightComplexity::ePointLight];
	for (auto& pl : myPointLightCommands)
	{
		bRC.myTransform.GetRow(4) = pl.myModelTransforms.GetRow(4);
		bRC.myEntityIndex = pl.myEntityIndex;
		sds->AddBillboardCommand(bRC);	//yuck
	}
	for (auto& scpl : myShadowPointLightCommands)
	{
		bRC.myTransform.GetRow(4) = scpl.myModelTransforms.GetRow(4);
		bRC.myEntityIndex = scpl.myEntityIndex;
		sds->AddBillboardCommand(bRC);	//yuck
	}

	bRC.myTextures = myLightIcons[(int)LightComplexity::eSpotLight];
	for (auto& sl : mySpotLightCommands)
	{
		bRC.myTransform.GetRow(4) = sl.myModelTransforms.GetRow(4);
		bRC.myEntityIndex = sl.myEntityIndex;
		sds->AddBillboardCommand(bRC);	//yuckYUVKUVK
		 
		lC.myFromPosition = sl.myModelTransforms.GetRow(4);
		lC.myToPosition = lC.myFromPosition + sl.myDirection * sl.myRange;
		dd->AddDynamicLineCommand(lC);

	}
	for (auto& scsl : myShadowSpotLightCommands)
	{
		bRC.myTransform.GetRow(4) = scsl.myModelTransforms.GetRow(4);
		bRC.myEntityIndex = scsl.myEntityIndex;
		sds->AddBillboardCommand(bRC);	//yuckYUVKUVK

		lC.myFromPosition = scsl.myModelTransforms.GetRow(4);
		lC.myToPosition = lC.myFromPosition + scsl.myDirection * scsl.myRange;
		dd->AddDynamicLineCommand(lC);
	}

	bRC.myTextures = myLightIcons[(int)LightComplexity::eAmbientLight];
	for (auto& al : myAmbientLightCommands)
	{
		bRC.myTransform.GetRow(4) = al.myModelToWorldTransforms.GetRow(4);
		bRC.myEntityIndex = al.myEntityIndex;

		sds->AddBillboardCommand(bRC);	//yuckYUVKUVK
	}

	bRC.myTextures = myLightIcons[(int)LightComplexity::eDirectionalLight];
	for (auto& dl : myDirectionalLightCommands)
	{
		bRC.myTransform.GetRow(4) = dl.myModelToWorldTransforms.GetRow(4);
		bRC.myEntityIndex = dl.myEntityIndex;
		sds->AddBillboardCommand(bRC);	//yuckYUVKUVK

		lC.myFromPosition = dl.myModelToWorldTransforms.GetRow(4);
		lC.myToPosition = lC.myFromPosition + dl.myModelToWorldTransforms.GetForward();
		dd->AddDynamicLineCommand(lC);
	}
}

void ISTE::LightDrawerSystem::RenderLights()
{ 
	myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthGreater, ReadWriteState::eRead);
	DrawDirectionalAndAmbientLight();

	myCtx->myRenderStateManager->SetRasterState(RasterizerState::eFrontFaceCulling);
	myCtx->myRenderStateManager->SetBlendState(BlendState::eAddativeBlend);
	myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthGreaterOrEqual, ReadWriteState::eRead);

	//ModelManager* mm = myCtx->myModelManager;

	DrawPointLights();
	DrawSpotLights();

	DrawShadowCastingSpotLights();
	DrawShadowCastingPointLights();
}

void ISTE::LightDrawerSystem::ClearRenderCommands()
{
	myPointLightCommands.clear();
	mySpotLightCommands.clear();
	myShadowPointLightCommands.clear();
	myShadowSpotLightCommands.clear();
	myAmbientLightCommands.clear();
	myDirectionalLightCommands.clear();
}	

void ISTE::LightDrawerSystem::SetDirectionalLight(EntityID aID)
{
	myActiveDirectionalLight = aID;

	DirectionalLightComponent* dlc = myCtx->mySceneHandler->GetActiveScene().GetComponent<DirectionalLightComponent>(myActiveDirectionalLight);
	dlc->myLightSpaceCam.SetProjectionMatrix(CU::Matrix4x4f::CreateOrthographicMatrix(-30, 30, -30, 30, -200.f, 500.f));
}


//lazy should move

struct Box
{
	CU::Vec3f myMin;
	CU::Vec3f myMax;

};


struct Sphere
{
	CU::Vec3f myPos;
	float myRadius;
};

//Assumes axis alligned box
bool SphereBoxIntersection(const Sphere& aSphere, const Box& aBox)
{
	CU::Vec3f pos = aSphere.myPos;
	CU::Vec3f LBB = aBox.myMin;
	CU::Vec3f RTF = aSphere.myPos + (aSphere.myRadius * (aBox.myMax - aBox.myMin)) / 2.f;
	
	CU::Vec3f closestPoint;
	
	closestPoint.x = max(LBB.x, min(RTF.x, pos.x));
	closestPoint.y = max(LBB.y, min(RTF.y, pos.y));
	closestPoint.z = max(LBB.z, min(RTF.z, pos.z));
	
	CU::Vec3f length = closestPoint - pos;
	
	if (length.Length() <= aSphere.myRadius)
	{
		return true;
	}
	
	return false;
	
	
	//if ((aSphere.myPos - aBox.myMin).Length() <= aSphere.myRadius) return true;
	//if ((aSphere.myPos - aBox.myMax).Length() <= aSphere.myRadius) return true;
	//
	// return false;
}



void ISTE::LightDrawerSystem::RecalculateLight(EntityID aId, float aRadius, CU::Vec3f aPos, std::set<EntityID>& aEntList)
{ 
	aEntList.clear();
	CullingSystem* cS = myCtx->mySystemManager->GetSystem<CullingSystem>(); 
	Cell*		grid = cS->GetGrid();
	CU::Vec3f	gridHalfSize;
	size_t		splits = UwUSplits * UwUSplits;
	size_t		i;
	Box			box;
	Sphere		sphere;


	sphere.myPos = aPos;
	sphere.myRadius = aRadius;

	std::vector<int> myIntersectedCells;
	myIntersectedCells.reserve(10);

	for (i = 0; i < splits; i++)
	{
		gridHalfSize = grid[i].mySize / 2.f;
		box.myMin = grid[i].myPosition - gridHalfSize;
		box.myMax = grid[i].myPosition + gridHalfSize;

		if (SphereBoxIntersection(sphere, box))
		{ 
			myIntersectedCells.emplace_back(i); 
		}
	}

	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& tPool = scene.GetComponentPool<TransformComponent>();
	
	TransformComponent* entityTransform;
	ModelComponent* modelComp;
	Model* model;

	EntityIndex index; 
	size_t size = myIntersectedCells.size();
	for (i = 0; i < size; i++)
	{
		for (auto& ent : grid[myIntersectedCells[i]].myEntities)
		{ 
			index = GetEntityIndex(ent);
			modelComp = (ModelComponent*)modelPool.Get(index);
			if (modelComp == nullptr)
				continue;
			if (modelComp->myModelId == ModelID(-1))
				continue;
			entityTransform = (TransformComponent*)tPool.Get(index);
			model = Context::Get()->myModelManager->GetModel(modelComp->myModelId);

			box.myMin = entityTransform->myPosition + (model->myAABB.myMin * entityTransform->myScale);
			box.myMax = entityTransform->myPosition + (model->myAABB.myMax * entityTransform->myScale);

			if (SphereBoxIntersection(sphere,box))
				aEntList.insert(ent);
		}
	}
}

void ISTE::LightDrawerSystem::UpdateShadowCastersEntityList(EntityID aId)
{
	//
	//this might be faster then doing get component from scene
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	EntityIndex index = GetEntityIndex(aId);

	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& spotPool = scene.GetComponentPool<ShadowCastingSpotLightComponent>();
	ComponentPool& pointPool = scene.GetComponentPool<ShadowCastingPointLightComponent>();
	ComponentPool& tPool = scene.GetComponentPool<TransformComponent>();


	TransformComponent* entityTransform = (TransformComponent*)tPool.Get(index);
	ShadowCastingSpotLightComponent* spotComp = (ShadowCastingSpotLightComponent*)spotPool.Get(index);
	ShadowCastingPointLightComponent* pointComp = (ShadowCastingPointLightComponent*)pointPool.Get(index);
	CU::Vec3f pos = entityTransform->myCachedTransform.GetTranslationV3();
	if (spotComp != nullptr)
	{ 
		RecalculateLight(aId, spotComp->myRange, pos, spotComp->myEntities);
		return;
	}
	if (pointComp != nullptr)
	{
		RecalculateLight(aId, pointComp->myRadius, pos, pointComp->myEntities);
		return;
	}
	
	
	
	ModelComponent* modelComp = (ModelComponent*)modelPool.Get(index);
	if (modelComp == nullptr)
		return;
	
	//gonna have to work out a solution for editor later
	Model* model = Context::Get()->myModelManager->GetModel(modelComp->myModelId);
	Box box;
	
	box.myMin = pos + (model->myAABB.myMin * entityTransform->myScale);
	box.myMax = pos + (model->myAABB.myMax * entityTransform->myScale);
	
	Sphere sphere;
	
	ComponentPool& sPLPool = scene.GetComponentPool<ShadowCastingPointLightComponent>();
	
	ShadowCastingPointLightComponent* sPLC = NULL;
	
	EntityIndex lightIndex = -1;
	
	bool pass;
	
	for (auto& sPL : myEntities[(int)LightComplexity::eShadowCastingPointLight])
	{
		pass = false;
		lightIndex = GetEntityIndex(sPL);
	
		sPLC = (ShadowCastingPointLightComponent*)sPLPool.Get(lightIndex);
	
		for (auto& id : sPLC->myEntities)
		{
			if (id == aId)
			{
				pass = true;
				break;
			}
		}
		
		if (pass)
			continue;
	
		sphere.myPos = ((TransformComponent*)tPool.Get(lightIndex))->myCachedTransform.GetTranslationV3();
		sphere.myRadius = sPLC->myRadius;
	
		if (SphereBoxIntersection(sphere, box))
			sPLC->myEntities.insert(aId);
	
	}
	
	ComponentPool& sSLPool = scene.GetComponentPool<ShadowCastingSpotLightComponent>();
	ShadowCastingSpotLightComponent* sSLC = NULL;
	
	for (auto& sSL : myEntities[(int)LightComplexity::eShadowCastingSpotLight])
	{
		pass = false;
		lightIndex = GetEntityIndex(sSL);
	
		sSLC = (ShadowCastingSpotLightComponent*)sSLPool.Get(lightIndex);
	
		for (auto& id : sSLC->myEntities)
		{
			if (id == aId)
			{
				pass = true;
				break;
			}
				
		}
	
		if (pass)
			continue;
	
		sphere.myPos = ((TransformComponent*)tPool.Get(lightIndex))->myCachedTransform.GetTranslationV3();
		sphere.myRadius = sSLC->myRange;
	
		if (SphereBoxIntersection(sphere, box))
			sSLC->myEntities.insert(aId);
	}
	
}

void ISTE::LightDrawerSystem::RemoveFromShadowCasterEntityList(EntityID aId)
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	ComponentPool& sPLPool = scene.GetComponentPool<ShadowCastingPointLightComponent>();
	ShadowCastingPointLightComponent* sPLC = NULL;

	EntityIndex lightIndex = -1;

	size_t counter;

	for (auto& sPL : myEntities[(int)LightComplexity::eShadowCastingPointLight])
	{
		counter = 0;
		lightIndex = GetEntityIndex(sPL);

		sPLC = (ShadowCastingPointLightComponent*)sPLPool.Get(lightIndex);

		for (auto& id : sPLC->myEntities)
		{
			if (id == aId)
			{
				sPLC->myEntities.erase(aId);
				break;
			}

			counter++;
		}
		
	}

	ComponentPool& sSLPool = scene.GetComponentPool<ShadowCastingSpotLightComponent>();
	ShadowCastingSpotLightComponent* sSLC = NULL;

	for (auto& sSL : myEntities[(int)LightComplexity::eShadowCastingSpotLight])
	{
		counter = 0;
		lightIndex = GetEntityIndex(sSL);

		sSLC = (ShadowCastingSpotLightComponent*)sSLPool.Get(lightIndex);

		for (auto& id : sSLC->myEntities)
		{
			if (id == aId)
			{
				sSLC->myEntities.erase(id);
				break;
			}

			counter++;

		}
	}
}

void ISTE::LightDrawerSystem::PrepareSpotLights()
{

	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& spotLightComponent = scene.GetComponentPool<SpotLightComponent>();

	for (auto entity : myEntities[(int)LightComplexity::eSpotLight])
	{
		SLC pCommand;
		pCommand.myEntityIndex = GetEntityIndex(entity);
		{
			ShadowCastingSpotLightComponent* lightData = (ShadowCastingSpotLightComponent*)spotLightComponent.Get(pCommand.myEntityIndex);
			pCommand.myColorAndIntensity = lightData->myColorAndIntensity;
			pCommand.myRange = lightData->myRange;
			pCommand.myInnerLimit = cos(lightData->myInnerAngle * ISTE::DegToRad);
			pCommand.myOuterLimit = cos(lightData->myOuterAngle * ISTE::DegToRad);
		}
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(pCommand.myEntityIndex);
			pCommand.myDirection = transformData->myCachedTransform.GetRow(3).GetNormalized();
			pCommand.myModelTransforms = transformData->myCachedTransform;
			pCommand.myModelTransforms.GetRow(1) *= pCommand.myRange + 2;
			pCommand.myModelTransforms.GetRow(2) *= pCommand.myRange + 2;
			pCommand.myModelTransforms.GetRow(3) *= pCommand.myRange + 2;
		}
		mySpotLightCommands.emplace_back(pCommand);
	}
}

void ISTE::LightDrawerSystem::PreparePointLights()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& pointLightComponent = scene.GetComponentPool<PointLightComponent>();

	for (auto entity : myEntities[(int)LightComplexity::ePointLight])
	{
		bool shadowCasting = false;
		PLC pCommand;
		pCommand.myEntityIndex = GetEntityIndex(entity);
		{
			PointLightComponent* lightData = (PointLightComponent*)pointLightComponent.Get(pCommand.myEntityIndex);
			pCommand.myRange = lightData->myRadius;
			pCommand.myColorAndIntensity = lightData->myColorAndIntensity;
			shadowCasting = lightData->myShadowCastingFlag;
		}
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(pCommand.myEntityIndex);
			pCommand.myModelTransforms(1, 1) = pCommand.myRange + 2;
			pCommand.myModelTransforms(2, 2) = pCommand.myRange + 2;
			pCommand.myModelTransforms(3, 3) = pCommand.myRange + 2;
			pCommand.myModelTransforms.GetRow(4) = transformData->myCachedTransform.GetRow(4);

		}
		myPointLightCommands.emplace_back(pCommand);
	}

}

void ISTE::LightDrawerSystem::PrepareShadowSpotLights()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& shadowCastingSpots = scene.GetComponentPool<ShadowCastingSpotLightComponent>();

	for (auto entity : myEntities[(int)LightComplexity::eShadowCastingSpotLight])
	{
		
		SLC& pCommand = myShadowSpotLightCommands.emplace_back(SLC());
		pCommand.myEntityIndex = GetEntityIndex(entity);
		{
			ShadowCastingSpotLightComponent* lightData = (ShadowCastingSpotLightComponent*)shadowCastingSpots.Get(pCommand.myEntityIndex);
			pCommand.myColorAndIntensity = lightData->myColorAndIntensity;
			pCommand.myRange = lightData->myRange;
			pCommand.myInnerLimit = cos(lightData->myInnerAngle * ISTE::DegToRad);
			pCommand.myOuterLimit = cos(lightData->myOuterAngle * ISTE::DegToRad);

			pCommand.myEnts = lightData->myEntities;

			CU::Matrix4x4f projectionMatrix;
			float nearPlane = 0.1f;
			float farPlane = pCommand.myRange + 20;
			float Q = farPlane / (farPlane - nearPlane);
			float fov = (lightData->myOuterAngle * 2) * ISTE::DegToRad;
			float S = 1.f / tan((fov) / 2.f);
			projectionMatrix(1, 1) = S;
			projectionMatrix(2, 2) = S;
			projectionMatrix(3, 3) = Q;
			projectionMatrix(4, 3) = (-Q) * nearPlane;
			projectionMatrix(3, 4) = 1;
			projectionMatrix(4, 4) = 0;
			pCommand.myCamera.SetProjectionMatrix(projectionMatrix); 
		}
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(pCommand.myEntityIndex);
			pCommand.myDirection = transformData->myCachedTransform.GetRow(3).GetNormalized();
			pCommand.myCamera.GetTransformNonConst() = transformData->myCachedTransform;
			pCommand.myModelTransforms = transformData->myCachedTransform;
			pCommand.myModelTransforms.GetRow(1) *= pCommand.myRange + 2;
			pCommand.myModelTransforms.GetRow(2) *= pCommand.myRange + 2;
			pCommand.myModelTransforms.GetRow(3) *= pCommand.myRange + 2;
		}
		myShadowSpotLightCommands.emplace_back(pCommand);
	}
}

void ISTE::LightDrawerSystem::PrepareShadowPointLights()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& shadowCastingPoints = scene.GetComponentPool<ShadowCastingPointLightComponent>();

	PLC* pCommand;
	for (auto entity : myEntities[(int)LightComplexity::eShadowCastingPointLight])
	{
		pCommand = &myShadowPointLightCommands.emplace_back(PLC());

		pCommand->myEntityIndex = GetEntityIndex(entity);
		{
			ShadowCastingPointLightComponent* lightData = (ShadowCastingPointLightComponent*)shadowCastingPoints.Get(pCommand->myEntityIndex);
			pCommand->myRange = lightData->myRadius;
			pCommand->myColorAndIntensity = lightData->myColorAndIntensity; 
			pCommand->myEnts = lightData->myEntities;
		}
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(pCommand->myEntityIndex);

			CU::Matrix4x4f projectionMatrix;
			float nearPlane = 0.01f;
			float farPlane = pCommand->myRange;
			float Q = farPlane / (farPlane - nearPlane);
			float fov = 90.f;
			float S = 1.f / tan((fov * ISTE::DegToRad) / 2.f);
			projectionMatrix(1, 1) = S;
			projectionMatrix(2, 2) = S;
			projectionMatrix(3, 3) = Q;
			projectionMatrix(4, 3) = (-Q) * nearPlane;
			projectionMatrix(3, 4) = 1;
			projectionMatrix(4, 4) = 0;

			pCommand->myCamera.SetProjectionMatrix(projectionMatrix);

			pCommand->myModelTransforms(1, 1) = pCommand->myRange + 2;
			pCommand->myModelTransforms(2, 2) = pCommand->myRange + 2;
			pCommand->myModelTransforms(3, 3) = pCommand->myRange + 2;
			pCommand->myModelTransforms.GetRow(4) = transformData->myCachedTransform.GetRow(4);
		}
	}
}

void ISTE::LightDrawerSystem::PrepareDirectionalLights()
{
	Scene& scene					= myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool	= scene.GetComponentPool<TransformComponent>();
	ComponentPool& dirLightPool		= scene.GetComponentPool<DirectionalLightComponent>(); 

	DLC lightCommand;
	for (auto entity : myEntities[(int)LightComplexity::eDirectionalLight])
	{
		lightCommand.myEntityIndex = GetEntityIndex(entity);

		TransformComponent* transformData = (TransformComponent*)transformPool.Get(lightCommand.myEntityIndex); 
		lightCommand.myModelToWorldTransforms = transformData->myCachedTransform;

		myDirectionalLightCommands.emplace_back(lightCommand);
	}
}

void ISTE::LightDrawerSystem::PrepareAmbientLights()
{
	Scene& scene					= myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool	= scene.GetComponentPool<TransformComponent>();
	ComponentPool& dirLightPool		= scene.GetComponentPool<AmbientLightComponent>();

	ALC lightCommand;
	for (auto entity : myEntities[(int)LightComplexity::eAmbientLight])
	{
		lightCommand.myEntityIndex = GetEntityIndex(entity);

		TransformComponent* transformData = (TransformComponent*)transformPool.Get(lightCommand.myEntityIndex);
		lightCommand.myModelToWorldTransforms = transformData->myCachedTransform;

		myAmbientLightCommands.emplace_back(lightCommand);
	}
}

void ISTE::LightDrawerSystem::DrawPointLights()
{
	ModelManager* mm = myCtx->myModelManager;  
	BindShader(LightComplexity::ePointLight);
	size_t size = myPointLightCommands.size();
	for (int i = 0; i < size; i++)
	{
		auto& com = myPointLightCommands[i];
		BindPointLightBuffer(com);
		mm->RenderModel(myPLightModel); 
	}  
}

void ISTE::LightDrawerSystem::DrawSpotLights()
{
	ModelManager* mm = myCtx->myModelManager;
	BindShader(LightComplexity::eSpotLight);
	size_t size = mySpotLightCommands.size();
	for (int i = 0; i < size; i++)
	{
		auto& com = mySpotLightCommands[i]; 
		BindSpotLightBuffer(com);
		mm->RenderModel(myPLightModel); 
	} 
}

void ISTE::LightDrawerSystem::DrawShadowCastingPointLights()
{
	ModelManager* mm = myCtx->myModelManager;
	size_t size = myShadowPointLightCommands.size();
	for (int i = 0; i < size; i++)
	{
		ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
		Context::Get()->myDX11->GetContext()->PSSetShaderResources(13, 1, nullSrv);

		myLightCubeDepthTarget.Clear(1);	
		auto& com = myShadowPointLightCommands[i];
		Camera* prevCam = &myCtx->myGraphicsEngine->GetCamera(); 

		//collects the depth of the cube texture
		//it will loop through all the 6 faces of the cube
		myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess);  
		myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBiasedRaster);
		myCtx->myRenderStateManager->SetBlendState(BlendState::eDisabled);
		//sdacom.myCamera.GetTransformNonConst

		for (int j = 0; j < 6; j++)
		{
			
			myLightCubeDepthTarget.SetDepthTargetActive(j);
			myPLTransforms[j].GetRow(4) = com.myModelTransforms.GetRow(4);
			com.myCamera.GetTransformNonConst() = myPLTransforms[j];
			myCtx->myGraphicsEngine->SetCamera(com.myCamera);
			myCtx->myGraphicsEngine->BindFrameBuffer();
			myCtx->myGraphicsEngine->RenderDepth(com.myEnts);
		}

		//reset and render the light
		myCtx->myGraphicsEngine->SetCamera(*prevCam);
		myCtx->myGraphicsEngine->BindFrameBuffer();
		myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthGreaterOrEqual, ReadWriteState::eRead);
		myCtx->myRenderStateManager->SetRasterState(RasterizerState::eFrontFaceCulling);
		myCtx->myRenderStateManager->SetBlendState(BlendState::eAddativeBlend);
		BindShader(LightComplexity::eShadowCastingPointLight);
		
		myCtx->myDX11->GetSceneTarget().SetActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV());
		myLightCubeDepthTarget.PsBind(13);
		BindPointLightBuffer(com);
		myCtx->myRenderStateManager->SetSampleState(AdressMode::eClamp, SamplerState::ePoint);
		mm->RenderModel(myPLightModel); 
	} 
}

void ISTE::LightDrawerSystem::DrawShadowCastingSpotLights()
{
	ModelManager* mm = myCtx->myModelManager;
	size_t size = myShadowSpotLightCommands.size();
	myCtx->myRenderStateManager->SetSampleState(AdressMode::eClamp, SamplerState::ePoint);
	
	for (int i = 0; i < size; i++)
	{
		ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
		Context::Get()->myDX11->GetContext()->PSSetShaderResources(12, 1, nullSrv);


		auto& com = myShadowSpotLightCommands[i];
		
		//collects depth 
		myLightDepthTarget.Clear();
		myLightDepthTarget.SetDepthTargetActive();  
		
		Camera* prevCam = &myCtx->myGraphicsEngine->GetCamera(); 
		
		myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess, ReadWriteState::eWrite);
		myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBackFaceCulling);
		myCtx->myRenderStateManager->SetBlendState(BlendState::eDisabled);
		
		myCtx->myGraphicsEngine->SetCamera(com.myCamera); 
		myCtx->myGraphicsEngine->BindFrameBuffer();
		myCtx->myGraphicsEngine->RenderDepth(com.myEnts);
		
		//reset and render the light
		myCtx->myDX11->GetSceneTarget().SetActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV());
		BindShader(LightComplexity::eShadowCastingSpotLight); 
		myCtx->myGraphicsEngine->SetCamera(*prevCam);
		myCtx->myGraphicsEngine->BindFrameBuffer();

		myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthGreaterOrEqual, ReadWriteState::eRead);
		myCtx->myRenderStateManager->SetRasterState(RasterizerState::eFrontFaceCulling);
		myCtx->myRenderStateManager->SetBlendState(BlendState::eAddativeBlend);

		myLightDepthTarget.PsBind(12);
		BindSpotLightBuffer(com);
		mm->RenderModel(myPLightModel);
	} 
}

void ISTE::LightDrawerSystem::DrawDirectionalAndAmbientLight()
{ 
	
	myCtx->myRenderStateManager->SetSampleState(AdressMode::eClamp, SamplerState::ePoint, 3);
	if (myActiveDirectionalLight != EntityID(-1) || myCtx->mySceneHandler->GetActiveScene().IsEntityIDValid(myActiveDirectionalLight))
	{
		DirectionalLightComponent* dirComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<DirectionalLightComponent>(myActiveDirectionalLight);
		if (dirComp != nullptr)
		{
			//collects depth 
			myLightDepthTarget.Clear();
			myLightDepthTarget.SetDepthTargetActive();
			myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBiasedRaster);

			//offsets the cam by main cameras position
			TransformComponent* transformComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myActiveDirectionalLight);
			Camera* prevCam = &myCtx->myGraphicsEngine->GetCamera();
			transformComp->myCachedTransform.GetRow(4) = prevCam->GetTransformNonConst().GetRow(4);
			dirComp->myLightSpaceCam.GetTransformNonConst() = transformComp->myCachedTransform;

			myCtx->myGraphicsEngine->SetCamera(dirComp->myLightSpaceCam);
			myCtx->myGraphicsEngine->BindFrameBuffer();
			myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess, ReadWriteState::eWrite);
			myCtx->myGraphicsEngine->RenderDepth();

			//reset and bind shadowmap
			myCtx->myGraphicsEngine->SetCamera(*prevCam);
			myCtx->myGraphicsEngine->BindFrameBuffer();
			
			myCtx->myDX11->GetSceneTarget().SetActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV());
			myLightDepthTarget.PsBind(12);

			myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBackFaceCulling);
			myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess, ReadWriteState::eRead);

			myCtx->myGraphicsEngine->GetLightBuffer().myLightSpaceMatrix = dirComp->myLightSpaceCam.GetWorldToClip();
			myCtx->myGraphicsEngine->GetLightBuffer().myDLdir = transformComp->myCachedTransform.GetForward();
			myCtx->myGraphicsEngine->GetLightBuffer().myDLColorAndIntensity = dirComp->myColorAndIntensity;
		}
	}

	if (myActiveAmbientLight != EntityID(-1) || myCtx->mySceneHandler->GetActiveScene().IsEntityIDValid(myActiveAmbientLight))
	{
		AmbientLightComponent* ambComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<AmbientLightComponent>(myActiveAmbientLight);
		TransformComponent* ambTfComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myActiveAmbientLight);
		if (ambComp != nullptr)
		{
			myCtx->myGraphicsEngine->GetLightBuffer().myALGroundColorAndIntensity = ambComp->mySkyColorAndIntensity; 
			myCtx->myGraphicsEngine->GetLightBuffer().myALSkyColorAndIntensity = ambComp->myGroundColorAndIntensity;
			myCtx->myGraphicsEngine->GetLightBuffer().myCubeMapIntensity = ambComp->myCubeMapIntensity;
			myCtx->myGraphicsEngine->GetLightBuffer().myAmbLightRotMatrix = ambTfComp->myCachedTransform;
			if(ambComp->myAmbianceTextureID != -1) //should set up a default cube map when this hits
				myCtx->myTextureManager->PsBindTexture(ambComp->myAmbianceTextureID, 3);
		}
	}

	myCtx->myGraphicsEngine->BindLightBuffer(); 
	myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPDirAmbLight).Render();
}

void ISTE::LightDrawerSystem::InitPointLightMatrixList()
{
	myPLTransforms[0](1, 1) = 0;
	myPLTransforms[0](3, 3) = 0;
	myPLTransforms[0](1, 3) = -1;
	myPLTransforms[0](2, 2) = 1;
	myPLTransforms[0](3, 1) = 1;

	myPLTransforms[1](1, 1) = 0;
	myPLTransforms[1](3, 3) = 0;
	myPLTransforms[1](1, 3) = 1;
	myPLTransforms[1](2, 2) = 1;
	myPLTransforms[1](3, 1) = -1;

	myPLTransforms[2](1, 1) = 1;
	myPLTransforms[2](2, 3) = -1;
	myPLTransforms[2](2, 2) = 0;
	myPLTransforms[2](3, 3) = 0;
	myPLTransforms[2](3, 2) = 1;

	myPLTransforms[3](1, 1) = 1;
	myPLTransforms[3](2, 3) = 1;
	myPLTransforms[3](2, 2) = 0;
	myPLTransforms[3](3, 3) = 0;
	myPLTransforms[3](3, 2) = -1;

	myPLTransforms[4](1, 1) = 1;
	myPLTransforms[4](2, 2) = 1;
	myPLTransforms[4](3, 3) = 1;

	myPLTransforms[5](1, 1) = -1;
	myPLTransforms[5](2, 2) = 1;
	myPLTransforms[5](3, 3) = -1;
}

void ISTE::LightDrawerSystem::BindPointLightBuffer(PLC& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	const ComPtr<ID3D11Buffer>& objBuffer = myObjBuffer[(int)LightComplexity::ePointLight];
	
	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(objBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	PointLightBuffer* OBdata = (PointLightBuffer*)(resource.pData);
	
	OBdata->myLightPerspectiveValues.x = aCommand.myCamera.GetProjectionMatrix()(3, 3);
	OBdata->myLightPerspectiveValues.y = aCommand.myCamera.GetProjectionMatrix()(4, 3);
	OBdata->myTransform = aCommand.myModelTransforms; 
	OBdata->myColorAndIntensity = aCommand.myColorAndIntensity;
	OBdata->myRange = aCommand.myRange;
	OBdata->Garb = aCommand.myRange - 0.01f;
	context->Unmap(objBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, objBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, objBuffer.GetAddressOf());
}

void ISTE::LightDrawerSystem::BindSpotLightBuffer(const SLC& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	const ComPtr<ID3D11Buffer>& objBuffer = myObjBuffer[(int)LightComplexity::eSpotLight];
	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(objBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	SpotLightBuffer* OBdata = (SpotLightBuffer*)(resource.pData);

	OBdata->myLightProjection	= aCommand.myCamera.GetWorldToClip();
	OBdata->myTransform			= aCommand.myModelTransforms;
	memcpy(&OBdata->myDirection.x, &aCommand.myDirection.x, sizeof(CU::Vec3f));

	OBdata->myColorAndIntensity = aCommand.myColorAndIntensity;
	OBdata->myInnerLimit		= aCommand.myInnerLimit;
	OBdata->myOuterLimit		= aCommand.myOuterLimit;
	OBdata->myRange				= aCommand.myRange;

	context->Unmap(objBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, objBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, objBuffer.GetAddressOf());
}

void ISTE::LightDrawerSystem::BindShader(LightComplexity aLightComplexity)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	context->VSSetShader(myVertexShader[(int)aLightComplexity].Get(), 0, 0);
	context->PSSetShader(myPixelShader[(int)aLightComplexity].Get(), 0, 0);
	context->IASetInputLayout(myInputLayout[(int)aLightComplexity].Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ISTE::LightDrawerSystem::InitShader(std::string aVSPath, std::string aPSPath, LightComplexity aLC)
{
	HRESULT result;
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	std::string vsData;
	DX11::CreateVertexShader(aVSPath, myVertexShader[(int)aLC].GetAddressOf(), vsData);
	DX11::CreatePixelShader(aPSPath, myPixelShader[(int)aLC].GetAddressOf());


	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"UV",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BINORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TANGENT",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BONES",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"WEIGHTS",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
		};

		int elementCount = sizeof(layout) / sizeof(layout[0]);
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout[(int)aLC].GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
} 
bool ISTE::LightDrawerSystem::InitPointLightBuffer()
{ 
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(PointLightBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)LightComplexity::ePointLight].GetAddressOf());
	if (FAILED(result))
		return false; 
	return true;
}

bool ISTE::LightDrawerSystem::InitSpotLightBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(SpotLightBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)LightComplexity::eSpotLight].GetAddressOf());
	if (FAILED(result))
		return false;
	return true;
} 