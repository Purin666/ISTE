#pragma once

#include "ISTE/Builder/ObjectBuilder.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Physics/EndLevelTrigger.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"

namespace ISTE
{
	class EndLevelTriggerBuilder : public ObjectBuilder
	{
	public:
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override
		{

			//TransformComponent* t = aScene.AssignComponent<TransformComponent>(aId);
			TriggerComponent* tr = aScene.AssignComponent<TriggerComponent>(aId);
			//std::string comp = typeid(TriggerComponent).name();
			aScene.AssignBehaviour<EndLevelTriggerBehaviour>(aId);

			//t->myPosition = aObject.myTransform.myPosition;
			//t->myQuaternion = aObject.myTransform.myRotation;
			//t->myScale = aObject.myTransform.myScale;

			//Context::Get()->myModelManager->LoadModel(aId, aObject.myModelData.myMeshPath);

			//for (int i = 0; i < aObject.myModelData.myAnimations.size(); i++)
			//{
			//	Context::Get()->myModelManager->LoadAnimation(aId, aObject.myModelData.myAnimations[i]);
			//}
		}

	private:
	};
}