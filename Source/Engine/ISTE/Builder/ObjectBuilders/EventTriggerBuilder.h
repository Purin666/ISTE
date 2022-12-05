#pragma once

#include "ISTE/Builder/ObjectBuilder.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/ModelManager.h"

// Trigger Types
#include "ISTE/Text/TextTrigger.h"
#include "ISTE/ComponentsAndSystems/BossTrigger.h"

namespace ISTE
{
	enum class TriggerTypes
	{
		eEmpty,
		eText,
		eModel,
		eBoss
	};

	class EventTriggerBuilder : public ObjectBuilder
	{
	public:
		void Create(EntityID aId, GObject& aObject, Scene& aScene) override
		{
			//TransformComponent* t = aScene.AssignComponent<TransformComponent>(aId);
			//
			//t->myPosition = aObject.myTransform.myPosition;
			//t->myQuaternion = aObject.myTransform.myRotation;
			//t->myScale = aObject.myTransform.myScale;

			switch ((TriggerTypes)aObject.myTriggerData.myType)
			{
			case TriggerTypes::eText:	
			{
				TextTrigger* tt = aScene.AssignBehaviour<TextTrigger>(aId);
				tt->myText = aObject.myTriggerData.myText;
				tt->myFont = aObject.myTriggerData.myFont;
				tt->myRadius = aObject.myTriggerData.myRadius;
				tt->myFollowTimer = aObject.myTriggerData.myFollowTimer;
				tt->myWriteSpeed = aObject.myTriggerData.myWriteSpeed;
				tt->myPageFlipDelay = aObject.myTriggerData.myPageFlipDelay;
				tt->myFontSize = aObject.myTriggerData.myFontSize;
				tt->myPageSize = aObject.myTriggerData.myPageSize;
				tt->myOneUse = aObject.myTriggerData.myOneUse;
				tt->myWillFollow = aObject.myTriggerData.myWillFollow;
				tt->myUseAnimation = aObject.myTriggerData.myUseAnimation;
				tt->myUsePages = aObject.myTriggerData.myUsePages;
				tt->myPages = aObject.myTriggerData.myPages;
				break;
			}
			case TriggerTypes::eModel:
			{
				break;
			}
			case TriggerTypes::eBoss:
			{
				BossTrigger* bt = aScene.AssignBehaviour<BossTrigger>(aId);
				bt->myRadius = aObject.myTriggerData.myRadius;
				bt->myPanTimer = aObject.myTriggerData.myPanTimer;
				bt->myWaitTimer = aObject.myTriggerData.myWaitTimer;
				bt->myDegreesToRotate = aObject.myTriggerData.myDegreesToRotate;
				bt->myUseCamera = aObject.myTriggerData.myUseCamera;
				bt->mySpawnPoolSize = aObject.myTriggerData.mySpawnPoolSize;
				bt->mySpawnPool = aObject.myTriggerData.mySpawnPool;
				break;
			}
			default:
				break;
			}

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