#include "EngineRegister.h"
#include "Context.h"
#include "Scene/Scene.h"
#include "Scene/SceneHandler.h"
#include "ECSB/SystemManager.h"
#include "Builder/SceneBuilder.h"

#pragma region "Component includes"
#include "Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"

//rendering 
#include "Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "Graphics/ComponentAndSystem/AnimationCommand.h"
#include "Graphics/ComponentAndSystem/CustomShaderComponent.h"
#include "Graphics/ComponentAndSystem/ModelComponent.h"
#include "Graphics/ComponentAndSystem/Sprite3DComponent.h"
#include "Graphics/ComponentAndSystem/Sprite2DComponent.h"
#include "Graphics/ComponentAndSystem/BillboardComponent.h"
#include "Graphics/ComponentAndSystem/VFXSprite3DComponent.h"
#include "Graphics/ComponentAndSystem/VFXSprite2DComponent.h"
#include "Graphics/ComponentAndSystem/TransperancyComponent.h"
#include "Graphics/ComponentAndSystem/PointLightComponent.h"
#include "Graphics/ComponentAndSystem/ShadowCastingPointLightComponent.h"
#include "Graphics/ComponentAndSystem/SpotLightComponent.h"
#include "Graphics/ComponentAndSystem/ShadowCastingSpotLightComponent.h"
#include "Graphics/ComponentAndSystem/DirectionalLightComponent.h"
#include "Graphics/ComponentAndSystem/AmbientLightComponent.h"  
#include "ISTE/Text/TextComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"

#include "ISTE/ComponentsAndSystems/LoDComponent.h"
#include "ISTE/VFX/SpriteParticles/EmitterComponent.h"
#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"

#pragma endregion

//behaviour includes
#pragma region "Behavior includes"

#include "ISTE/Audio/AudioSource.h"
#include "ISTE/Audio/EventAudioSource.h"
#include "ISTE/ECSB/PotBehaviour.h"
#include "ISTE/ECSB/ProjectileBlockBehaviour.h"

#pragma endregion
//

//system includes
#pragma region "System includes"
#include "Graphics/ComponentAndSystem/TransformSystem.h"

#include "ComponentsAndSystems/BehaviourSystem.h"

#include "ISTE/Physics/ComponentsAndSystems/TriggerSystem.h"

#include "ISTE/Graphics/ComponentAndSystem/CullingSystem.h"

#include "ISTE/ComponentsAndSystems/EnemyBehaviourSystem.h"

//rendering systems
#include "Graphics/ComponentAndSystem/MaterialComponent.h"
#include "Graphics/ComponentAndSystem/AnimationDrawerSystem.h"
#include "Graphics/ComponentAndSystem/CustomDrawerSystem.h"
#include "Graphics/ComponentAndSystem/ModelDrawerSystem.h"
#include "Graphics/ComponentAndSystem/TransperancyDrawerSystem.h"
#include "Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "Graphics/ComponentAndSystem/VFXSpriteDrawerSystem.h"
#include "Graphics/ComponentAndSystem/CameraSystem.h"
#include "Graphics/ComponentAndSystem/LightDrawerSystem.h"
#include "Graphics/ComponentAndSystem/DecalDrawerSystem.h"
#include "ISTE/Text/TextSystem.h"

#include "ISTE/Audio/AudioSourceSystem.h"
#include "ISTE/ComponentsAndSystems/LoDSystem.h"
#include "VFX/SpriteParticles/EmitterSystem.h"
#pragma endregion
//

#pragma region "ObjectBuilder includes"

#include "Builder/ObjectBuilders/TestBuilder.h"
#include "Builder/ObjectBuilders/PlayerBuilder.h"
#include "Builder/ObjectBuilders/EnemyBuilder.h"
#include "Builder/ObjectBuilders/EndLevelTriggerBuilder.h"
#include "Builder/ObjectBuilders/EventTriggerBuilder.h"
#include "Builder/ObjectBuilders/DecalBuilder.h"
#include "Builder/ObjectBuilders/PotBuilder.h"

#pragma endregion

#pragma region CRAWs

#include "ISTE/Builder/ComponentReadAndWrites/TransformComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/ModelComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/PointLightComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/ShadowPointLightComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/SpotLightComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/ShadowSpotLightComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/DirectionalLightComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/AmbientLightCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/PlayerBehaviourCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/AudioSourceBehaviourCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/DecalCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/EmitterComponentCRAW.h"
#include "ISTE/Builder/ComponentReadAndWrites/MaterialComponentCRAW.h"

#pragma endregion

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{
	void EngineRegister::Init()
	{
		myCtx = Context::Get();
		mySceneHandler = myCtx->mySceneHandler;
		mySystemManager = myCtx->mySystemManager;

		//RegisterComponents();
		//RegisterBehaviours();
		//RegisterSystems();

	}
	void EngineRegister::RegisterComponents()
	{
		//call scenehandler
		mySceneHandler->RegisterComponent<TransformComponent>();
		mySceneHandler->RegisterComponent<TriggerComponent>();
		mySceneHandler->RegisterComponent<SphereTriggerComponent>();
		mySceneHandler->RegisterComponent<AttackValueComponent>();
		mySceneHandler->RegisterComponent<EnemyStatsComponent>();
		mySceneHandler->RegisterComponent<LoDComponent>();
		mySceneHandler->RegisterComponent<EmitterComponent>();
		 
#pragma region"Rendering Components"
		mySceneHandler->RegisterComponent<MaterialComponent>();
		mySceneHandler->RegisterComponent<ModelComponent>();
		mySceneHandler->RegisterComponent<Sprite3DComponent>();
		mySceneHandler->RegisterComponent<Sprite2DComponent>();
		mySceneHandler->RegisterComponent<BillboardComponent>();
		mySceneHandler->RegisterComponent<VFXSprite3DComponent>();
		mySceneHandler->RegisterComponent<VFXSprite2DComponent>();
		mySceneHandler->RegisterComponent<CustomShaderComponent>();
		mySceneHandler->RegisterComponent<TransperancyComponent>();
		mySceneHandler->RegisterComponent<AnimatorComponent>();
		mySceneHandler->RegisterComponent<TextComponent>();
		mySceneHandler->RegisterComponent<DecalComponent>();
		mySceneHandler->RegisterComponent<AnimationBlendComponent>();
#pragma endregion

#pragma region"Light Components"
		mySceneHandler->RegisterComponent<PointLightComponent>();
		mySceneHandler->RegisterComponent<ShadowCastingPointLightComponent>();
		mySceneHandler->RegisterComponent<SpotLightComponent>();
		mySceneHandler->RegisterComponent<ShadowCastingSpotLightComponent>();
		mySceneHandler->RegisterComponent<AmbientLightComponent>();
		mySceneHandler->RegisterComponent<DirectionalLightComponent>();

#pragma endregion
	}
	void EngineRegister::RegisterBehaviours()
	{
		//call scenehandler

		mySceneHandler->RegisterBehaviour<AudioSource>();
		mySceneHandler->RegisterBehaviour<EventAudioSource>();
		mySceneHandler->RegisterBehaviour<PotBehaviour>();
		mySceneHandler->RegisterBehaviour<ProjectileBlockBehaviour>();
	}
	void EngineRegister::RegisterSystems()
	{
		//call systemmanager



		ComponentMask mask;
		ComponentMask ignoreMask;
		mask.set(mySceneHandler->GetId<TransformComponent>()); 
		mySystemManager->RegisterSystem<TransformSystem>();
		mySystemManager->AddComplexity<TransformSystem>(mask, 0, false, ComponentMask(), false, true);
		mask.reset();
		
		mask.set(mySceneHandler->GetId<TriggerComponent>());
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mySystemManager->RegisterSystem<TriggerSystem>();
		mySystemManager->AddComplexity<TriggerSystem>(mask, (int)TriggerSystem::MapValues::Box);
		mask.reset();
		mask.set(mySceneHandler->GetId<SphereTriggerComponent>());
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mySystemManager->AddComplexity<TriggerSystem>(mask, (int)TriggerSystem::MapValues::Sphere);
		mask.reset();

#pragma region"Renderers"
		//sprite
		mySystemManager->RegisterSystem<SpriteDrawerSystem>();
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<Sprite2DComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mySystemManager->AddComplexity<SpriteDrawerSystem>(mask,0,false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<Sprite3DComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mySystemManager->AddComplexity<SpriteDrawerSystem>(mask, 1, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<BillboardComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mySystemManager->AddComplexity<SpriteDrawerSystem>(mask, 2, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		// Decal Drawer
		mySystemManager->RegisterSystem<DecalDrawerSystem>();
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<DecalComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mySystemManager->AddComplexity<DecalDrawerSystem>(mask, 0, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();


		// VFX sprite
		mySystemManager->RegisterSystem<VFXSpriteDrawerSystem>();
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<VFXSprite2DComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mySystemManager->AddComplexity<VFXSpriteDrawerSystem>(mask, 0, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<VFXSprite3DComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mySystemManager->AddComplexity<VFXSpriteDrawerSystem>(mask, 1, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();
		
		//Text system
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<TextComponent>());

		mySystemManager->RegisterSystem<TextSystem>();
		mySystemManager->AddComplexity<TextSystem>(mask);
		mask.reset();

		//models
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ModelComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		 
		ignoreMask.set(mySceneHandler->GetId<AnimatorComponent>());  
		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		ignoreMask.set(mySceneHandler->GetId<TransperancyComponent>());

		mySystemManager->RegisterSystem<ModelDrawerSystem>();
		mySystemManager->AddComplexity<ModelDrawerSystem>(mask, 0, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		//animator 
		mask.set(mySceneHandler->GetId<AnimatorComponent>());
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ModelComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());

		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		ignoreMask.set(mySceneHandler->GetId<TransperancyComponent>());
		ignoreMask.set(mySceneHandler->GetId<AnimationBlendComponent>());

		mySystemManager->RegisterSystem<AnimationDrawerSystem>();
		mySystemManager->AddComplexity<AnimationDrawerSystem>(mask, 0, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		mask.set(mySceneHandler->GetId<AnimatorComponent>());
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ModelComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		mask.set(mySceneHandler->GetId<AnimationBlendComponent>());

		ignoreMask.set(mySceneHandler->GetId<CustomShaderComponent>());
		ignoreMask.set(mySceneHandler->GetId<TransperancyComponent>());

		mySystemManager->RegisterSystem<AnimationDrawerSystem>();
		mySystemManager->AddComplexity<AnimationDrawerSystem>(mask, 1, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		//Custom Renderer
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<CustomShaderComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		 
		ignoreMask.set(mySceneHandler->GetId<TransperancyComponent>()); 

		mySystemManager->RegisterSystem<CustomDrawerSystem>();
		mySystemManager->AddComplexity<CustomDrawerSystem>(mask, 0, false, ignoreMask);
		mask.reset();
		ignoreMask.reset();

		//Transparancy
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<TransperancyComponent>());
		mask.set(mySceneHandler->GetId<MaterialComponent>());
		
		mySystemManager->RegisterSystem<TransperancyDrawerSystem>();
		mySystemManager->AddComplexity<TransperancyDrawerSystem>(mask);
		mask.reset(); 

#pragma endregion

#pragma region"Light"

		mySystemManager->RegisterSystem<LightDrawerSystem>();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<DirectionalLightComponent>());
		mySystemManager->AddComplexity<LightDrawerSystem>(mask,0);
		mask.reset();
		
		mask.set(mySceneHandler->GetId<AmbientLightComponent>());
		mySystemManager->AddComplexity<LightDrawerSystem>(mask,1);
		mask.reset();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<PointLightComponent>());
		mySystemManager->AddComplexity<LightDrawerSystem>(mask,2);
		mask.reset();
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ShadowCastingPointLightComponent>());
		mySystemManager->AddComplexity<LightDrawerSystem>(mask, 3);
		mask.reset();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<SpotLightComponent>());
		mySystemManager->AddComplexity<LightDrawerSystem>(mask,4);
		mask.reset();
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ShadowCastingSpotLightComponent>());
		mySystemManager->AddComplexity<LightDrawerSystem>(mask, 5);
		mask.reset();

#pragma endregion
		//Camera
		mask.set(mySceneHandler->GetId<TransformComponent>());

		mySystemManager->RegisterSystem<CameraSystem>();
		mySystemManager->AddComplexity<CameraSystem>(mask);
		mask.reset();

		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ModelComponent>());
		mySystemManager->RegisterSystem<CullingSystem>();
		mySystemManager->AddComplexity<CullingSystem>(mask, 0, false, ComponentMask(), true);
		mask.reset();

		mask.set(mySceneHandler->GetId<AudioSource>());
		mySystemManager->RegisterSystem<AudioSourceSystem>();
		mySystemManager->AddComplexity<AudioSourceSystem>(mask, 0, false, ComponentMask(), true);
		mask.reset();

		mask.set(mySceneHandler->GetId<LoDComponent>());
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mask.set(mySceneHandler->GetId<ModelComponent>()); //for now just guarante that everything needed is there
		mySystemManager->RegisterSystem<LoDSystem>();
		mySystemManager->AddComplexity<LoDSystem>(mask, 0, false, ComponentMask(), true);
		mask.reset();

		mask.set(mySceneHandler->GetId<EmitterComponent>());
		mask.set(mySceneHandler->GetId<TransformComponent>());
		mySystemManager->RegisterSystem<EmitterSystem>();
		mySystemManager->AddComplexity<EmitterSystem>(mask);
		mask.reset();

		mask.set(mySceneHandler->GetId<IdleEnemyBehaviour>());
		mask.set(mySceneHandler->GetId<BossBobBehaviour>());
		mask.set(mySceneHandler->GetId<PotBehaviour>());
		mySystemManager->RegisterSystem<EnemyBehaviourSystem>();
		mySystemManager->AddComplexity<EnemyBehaviourSystem>(mask, 0, true);
		mask.reset();

		mySystemManager->RegisterSystem<BehaviourSystem>();
		mySystemManager->AddComplexity<BehaviourSystem>(mySceneHandler->myFBM, 0, true);
	}

	void EngineRegister::RegisterObjectBuilders()
	{
		SceneBuilder* builder = mySceneHandler->GetSceneBuilder();
		
		builder->RegisterObjectBuilder<TestBuilder>("Test");
		builder->RegisterObjectBuilder<PlayerBuilder>("Player");
		builder->RegisterObjectBuilder<EnemyBuilder>("EnemyMelee");
		builder->RegisterObjectBuilder<EnemyBuilder>("EnemyMeleeElite");
		builder->RegisterObjectBuilder<EnemyBuilder>("EnemyRanged");
		builder->RegisterObjectBuilder<EnemyBuilder>("EnemyRangedElite");
		builder->RegisterObjectBuilder<EnemyBuilder>("BossBob");
		builder->RegisterObjectBuilder<EndLevelTriggerBuilder>("EndLevelTrigger");
		builder->RegisterObjectBuilder<EventTriggerBuilder>("EventTrigger");
		builder->RegisterObjectBuilder<DecalBuilder>("Decal");
		builder->RegisterObjectBuilder<PotBuilder>("PotSmol");
		builder->RegisterObjectBuilder<PotBuilder>("PotMedium");
		builder->RegisterObjectBuilder<PotBuilder>("PotGiga");
	}
	void EngineRegister::RegisterReadAndWrites()
	{
		SceneBuilder* builder = mySceneHandler->myBuilder;

		builder->RegisterComponentReadAndWrite<TransformComponentCRAW, TransformComponent>();
		builder->RegisterComponentReadAndWrite<ModelComponentCRAW, ModelComponent>();
		builder->RegisterComponentReadAndWrite<PointLightComponentCRAW, PointLightComponent>();
		builder->RegisterComponentReadAndWrite<ShadowPointLightComponentCRAW, ShadowCastingPointLightComponent>();
		builder->RegisterComponentReadAndWrite<SpotLightComponentCRAW, SpotLightComponent>();
		builder->RegisterComponentReadAndWrite<ShadowSpotLightComponentCRAW, ShadowCastingSpotLightComponent>();
		builder->RegisterComponentReadAndWrite<DirectionalLightComponentCRAW, DirectionalLightComponent>();
		builder->RegisterComponentReadAndWrite<AmbientLightCRAW, AmbientLightComponent>();
		builder->RegisterComponentReadAndWrite<PlayerBehaviourCRAW, PlayerBehaviour>();
		builder->RegisterComponentReadAndWrite<AudioSourceBehaviourCRAW, AudioSource>();
		builder->RegisterComponentReadAndWrite<DecalComponentCRAW, DecalComponent>();
		builder->RegisterComponentReadAndWrite<EmitterComponentCRAW, EmitterComponent>();
		builder->RegisterComponentReadAndWrite<MaterialComponentCRAW, MaterialComponent>();
	}
}