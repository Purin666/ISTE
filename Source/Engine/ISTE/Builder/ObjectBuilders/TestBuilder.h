#pragma once

#include "../ObjectBuilder.h"

#include "../../Scene/Scene.h"
#include "../../Graphics/ComponentAndSystem/TransformComponent.h"
#include "../../Context.h"
#include "../../Graphics/Resources/ModelManager.h"

namespace ISTE
{
	class TestBuilder : public ObjectBuilder
	{
	public:
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override
		{

			//TransformComponent* t = aScene.AssignComponent<TransformComponent>(aId);
			//
			//t->myPosition = aObject.myTransform.myPosition;
			//t->myQuaternion = aObject.myTransform.myRotation;
			//t->myScale = aObject.myTransform.myScale;
			//
			//Context::Get()->myModelManager->LoadModel(aId, aObject.myModelData.myMeshPath);
			//
			//for (int i = 0; i < aObject.myModelData.myAnimations.size(); i++)
			//{
			//	Context::Get()->myModelManager->LoadAnimation(aId, aObject.myModelData.myAnimations[i]);
			//}
		}

	private:
	};
}