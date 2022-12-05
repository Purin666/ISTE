#pragma once

#include "../ObjectBuilder.h"

#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "../../Context.h"
#include "../../Graphics/Resources/ModelManager.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/ECSB/PotBehaviour.h"
#include "ISTE/Audio/AudioSource.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/CU/UtilityFunctions.hpp"

namespace ISTE
{
	class PotBuilder : public ObjectBuilder
	{
	public:
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override
		{
			TransformComponent* transformComp = aScene.GetComponent<TransformComponent>(aId);
			ModelComponent* modelComp = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(aId);
			Model* model = Context::Get()->myModelManager->GetModel(modelComp->myModelId);
			TriggerComponent* triggerComp = aScene.AssignComponent<TriggerComponent>(aId);
			AudioSource* audioSource = aScene.AssignBehaviour<AudioSource>(aId);

			//tComp->mySize = CU::Vec3f(4, 4, 4);
			triggerComp->myOffset.y += (model->myAABB.myMax.y - model->myAABB.myMin.y) * 0.5f;

			PotBehaviour* pot = aScene.AssignBehaviour<PotBehaviour>(aId);
			pot->myActiveFlag = true;

			if (modelComp)
				modelComp->myColor += CU::Vec3f(3.f, 3.f, 3.f);

			transformComp->myEuler.AddRotation(CU::Vec3f(0.f, CU::GetRandomFloat(0.f, 360.f), 0.f));

			if (aObject.myTag == "PotSmol")
			{
				pot->myAnimation = Context::Get()->myAnimationManager->LoadAnimation(aId, "../Assets/Animations/EN_P_Pot_Broken2.fbx");
				pot->myAnimationHelper.SetEntityID(aId);
				pot->myAnimationHelper.MapAnimation(0, pot->myAnimation, 0, 0, 0, 10.f);
				pot->myAnimationHelper.Stop();
			}
			if (aObject.myTag == "PotMedium")
			{
				pot->myAnimation = Context::Get()->myAnimationManager->LoadAnimation(aId, "../Assets/Animations/EN_P_Pot_Broken.fbx");
				pot->myAnimationHelper.SetEntityID(aId);
				pot->myAnimationHelper.MapAnimation(0, pot->myAnimation, 0, 0, 0, 10.f);
				pot->myAnimationHelper.Stop();
			}

			//lazy ting
			pot->myTag = aObject.myTag;

			if (audioSource)
			{
				audioSource->LoadAndMapClip(0, "../Assets/Audio/Sounds/PotBreak.wav", false, true);
			}
		}

	private:
	};
}