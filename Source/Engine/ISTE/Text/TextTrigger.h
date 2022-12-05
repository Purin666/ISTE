#pragma once

#include "ISTE/ECSB/behaviour.h"
#include <string>
#include <vector>

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h"
#include "ISTE/Math/Vec3.h"

namespace ISTE
{
	class Scene;
	class TextTrigger : public Behaviour
	{
	public:

		void Init() override;

		void Update(float aDeltaTime) override;

	private:
		friend class EventTriggerBuilder;

		ISTE::Scene* myScene;

		ISTE::Sprite2DRenderCommand myBox;

		std::string myText;
		std::string myTempText;
		std::string myFont;
		
		CU::Vec3f myOffset;

		float myRadius;
		float myFollowTimer;
		float myWriteSpeed;
		float myPageFlipDelay;

		int myFontSize;
		int myStringIndex;
		int myPageSize;
		int myPageIndex;

		bool myOneUse;
		bool myHasUpdated;
		bool myIsInactive;
		bool myWillFollow;
		bool myUnderFollowTimer;
		bool myUseAnimation;
		bool myCanUseNextIndex;
		bool myUsePages;
		bool myCanFlipPage;
		bool myRenderBox;

		std::vector<std::string> myPages;

		EntityID myTextId;
	};
}