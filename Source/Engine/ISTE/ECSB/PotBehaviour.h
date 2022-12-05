#pragma once
#include "Behaviour.h"

#include "ISTE/Graphics/Helper/AnimationHelper.h"

namespace ISTE
{
	class PotBehaviour : public Behaviour
	{
	public:

		void Init() override;
		void Update(float aDeltaTime) override;

		void OnTrigger(EntityID) override;

	private:
		friend class PotBuilder;
		AnimationHelper<int> myAnimationHelper;
		AnimationID myAnimation;

		std::string myTag = "";

		bool myActiveFlag;
	};
}