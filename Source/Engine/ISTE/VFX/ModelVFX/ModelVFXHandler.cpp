#include "ModelVFXHandler.h"

#include "ISTE/VFX/ModelVFX/ModelVFXDefines.h"

#include "ISTE/Context.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"

#include "ISTE/CU/UtilityFunctions.hpp"
#include "ISTE/VFX/VFXUtilityFunctions.h"


#include <string>

#include "ISTE/CU/MemTrack.hpp"

const EntityID ISTE::ModelVFXHandler::Spawn(const std::string& aName, const CU::Vec3f& aPosition, const CU::Vec3f& someEulerAngles)
{
	// Check if model exists, dont wanna spawn if not
	auto itr = mySystem->myDatas.find(aName);
	if (itr == mySystem->myDatas.end())
	{
		std::cout << "Error in " << __func__ << ": No ModelVFX type with name " << aName << std::endl;
		return 0;
	}

	Context* ctx = Context::Get();
	auto& data = mySystem->myDatas.at(aName);

	// New entity
	Scene& scene = ctx->mySceneHandler->GetActiveScene();
	EntityID id = scene.NewEntity();
	scene.SetLayer(id, "IgnoreVisibility");
	mySystem->myVFX[id] = { id };
	auto& vfx = mySystem->myVFX[id];
	vfx.myTypeName = aName;

	// Assign components
	TransformComponent* transform = scene.AssignComponent<TransformComponent>(id);
	ModelComponent* model = scene.AssignComponent<ModelComponent>(id);
	MaterialComponent* material = scene.AssignComponent<MaterialComponent>(id);
	CustomShaderComponent* shader = scene.AssignComponent<CustomShaderComponent>(id);

	// Standard
	shader->myShader = ISTE::Shaders::eDefaultVFXModelShader;
	model->myAdressMode = ISTE::AdressMode::eWrap;
	model->mySamplerState = ISTE::SamplerState::eTriLinear;

	// Model data
	model->myModelId = ctx->myModelManager->LoadModel(data.myModelPath);

	// Texture data
	const size_t textureCount = data.myTexturePath.size();
	for (size_t i = 0; i < textureCount; i++)
	{
		material->myTextures[0][i] = ctx->myTextureManager->LoadTexture(std::wstring(data.myTexturePath[i].begin(), data.myTexturePath[i].end())).myValue;
		shader->myUvChangeOverTimes[i] = data.myUVOverLife[i];
	}
	// sample
	// fade x
	// fade y

	// Generic
	vfx.myLifetime = Rand(data.myLifetime);
	shader->myDuration = vfx.myLifetime; // also set the shader duration because it will be constant
	vfx.myDelay = Rand(data.myDelay);
	vfx.myFirstDelay = Rand(data.myFirstDelay);
	vfx.myShouldLoop = data.myShouldLoop;

	// Color
	vfx.myColor = data.myInitialColor;
	// color over life

	// Size
	vfx.mySize = Rand(data.myInitialSize);

	// Rotation
	vfx.myRotation = someEulerAngles + Rand(data.myInitialRotation);

	// Location
	vfx.myPosition = aPosition;
	vfx.myPosition += Rand(data.myInitialBoxPosition);
	vfx.myPosition += Rand(data.myInitialSpherePosition) * RandPointInsideUnitSphere();

	// Physics
	vfx.myVelocity = Rand(data.myInitialVelocity);
	vfx.myConstAcceleration = Rand(data.myConstAcceleration);
	vfx.myMass = Rand(data.myMass);
	vfx.myResistance = Rand(data.myResistance);

	return id;
}

void ISTE::ModelVFXHandler::Attach(const EntityID aVFX, const EntityID anOther)
{
	auto itr = mySystem->myVFX.find(aVFX);
	if (itr != mySystem->myVFX.end())
		itr->second.myAttachedID = anOther;
}

void ISTE::ModelVFXHandler::SetPosition(const EntityID aVFX, const CU::Vec3f& aPosition)
{
	auto itr = mySystem->myVFX.find(aVFX);
	if (itr != mySystem->myVFX.end())
	{
		itr->second.myPosition = aPosition;
	}
}

void ISTE::ModelVFXHandler::SetRotation(const EntityID aVFX, const CU::Vec3f& someEulerAngles)
{	
	auto itr = mySystem->myVFX.find(aVFX);
	if (itr != mySystem->myVFX.end())
	{
		itr->second.myRotation = someEulerAngles;
	}
}

void ISTE::ModelVFXHandler::Kill(const EntityID aVFX)
{
	auto itr = mySystem->myVFX.find(aVFX);
	if (itr != mySystem->myVFX.end())
	{
		// remove from scene
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
		scene.DestroyEntity(aVFX);
		
		// remove from my system
		mySystem->myVFX.erase(itr);
	}
}

void ISTE::ModelVFXHandler::KillAll()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	for (auto& [id, vfx] : mySystem->myVFX)
	{
		scene.DestroyEntity(vfx.myEntityID);
	}
	mySystem->myVFX.clear();
}

void ISTE::ModelVFXHandler::Update(const float aTimeDelta)
{
	if (!myPauseFlag)
		UpdateInstances(aTimeDelta);

	UpdateComponents();
}

void ISTE::ModelVFXHandler::UpdateInstances(const float aTimeDelta)
{
	for (auto& [id, vfx] : mySystem->myVFX)
	{
		auto& data = mySystem->myDatas.at(vfx.myTypeName);

		if ((vfx.myRelativeTime += aTimeDelta) > vfx.myLifetime)
		{
			if (vfx.myShouldLoop)
				vfx.myRelativeTime = 0.f;
			else
				myKillQueue.push(id);
			continue;
			//else
			//	Kill(id);
			//break;
		}

		const float t = vfx.myRelativeTime / vfx.myLifetime;

		// color
		if (!data.myColorOverLife.empty())
			vfx.myColor = PiecewiseLinearOnUnitInterval(t, data.myColorOverLife.data(), data.myColorOverLife.size());

		// position
		vfx.myPosition += vfx.myVelocity * aTimeDelta;

		// size
		if (!data.mySizeOverLife.empty())
			vfx.mySize = PiecewiseLinearOnUnitInterval(t, data.mySizeOverLife.data(), data.mySizeOverLife.size());

		// rotation
		if (!data.myRotationRateOverLife.empty())
			vfx.myRotation += PiecewiseLinearOnUnitInterval(t, data.myRotationRateOverLife.data(), data.myRotationRateOverLife.size()) * aTimeDelta;
		
		// velocity
		vfx.myVelocity += vfx.myConstAcceleration * aTimeDelta;
		vfx.myVelocity.y += vfx.myMass * Gravity * aTimeDelta;
		vfx.myVelocity *= 1.f - vfx.myResistance * aTimeDelta;
	}
	while (!myKillQueue.empty())
	{
		Kill(myKillQueue.front());
		myKillQueue.pop();
	}
}

void ISTE::ModelVFXHandler::UpdateComponents()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& shaderPool = scene.GetComponentPool<CustomShaderComponent>();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();

	CustomShaderComponent* shader = nullptr;
	TransformComponent* transform = nullptr;
	ModelComponent* model = nullptr;
	EntityIndex entityIndex = -1;

	for (auto& [id, vfx] : mySystem->myVFX)
	{
		auto& data = mySystem->myDatas.at(vfx.myTypeName);

		if (!scene.IsEntityIDValid(vfx.myEntityID)) continue;

		entityIndex = GetEntityIndex(vfx.myEntityID);
		
		transform = (TransformComponent*)transformPool.Get(entityIndex);
		model = (ModelComponent*)modelPool.Get(entityIndex);
		shader = (CustomShaderComponent*)shaderPool.Get(entityIndex);

		if (transform == nullptr) continue;
		if (model == nullptr) continue;
		if (shader == nullptr) continue;

		// Transform
		transform->myPosition = vfx.myPosition;
		transform->myEuler.SetRotation(vfx.myRotation);
		transform->myScale = vfx.mySize;
		if (IsEntityIndexValid(vfx.myAttachedID))
		{
			TransformComponent* other = (TransformComponent*)transformPool.Get(GetEntityIndex(vfx.myAttachedID));
			transform->myPosition = other->myPosition;
		}

		// Model
		model->myColor = vfx.myColor;

		// Shader
		shader->myElaspedTime = vfx.myRelativeTime;
		const float t = vfx.myRelativeTime / vfx.myLifetime;
		for (size_t i = 0; i < data.myUVOverLife.size(); i++)
		{
			shader->myUvChangeOverTimes[i] = data.myUVOverLife[i];
			shader->mySamples[i] = data.mySample[i];
		}

		// update model and material, used to change real time
		{
			model->myModelId = Context::Get()->myModelManager->LoadModel(data.myModelPath);
			
			MaterialComponent* matComp = scene.AssignComponent<MaterialComponent>(id);
			for (size_t i = 0; i < data.myTexturePath.size(); i++)
				matComp->myTextures[0][i] = Context::Get()->myTextureManager->LoadTexture(std::wstring(data.myTexturePath[i].begin(), data.myTexturePath[i].end())).myValue;
		}
	}
}

void ISTE::ModelVFXHandler::LoadModelsAndTextures()
{
	LoadModels();
	LoadTextures();
}

void ISTE::ModelVFXHandler::LoadModels()
{
	auto mgr = Context::Get()->myModelManager;
	for (auto& [name, data] : mySystem->myDatas)
	{
		mgr->LoadModel(data.myModelPath);
	}
}

void ISTE::ModelVFXHandler::LoadTextures()
{
	auto mgr = Context::Get()->myTextureManager;
	for (auto& [name, data] : mySystem->myDatas)
	{
		for (size_t i = 0; i < data.myTexturePath.size(); i++)
		{
			mgr->LoadTexture(std::wstring(data.myTexturePath[i].begin(), data.myTexturePath[i].end()));
		}
	}
}
