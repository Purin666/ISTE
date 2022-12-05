#pragma once

#include "ISTE/ECSB/behaviour.h"

namespace ISTE
{
	class EndLevelTriggerBehaviour : public Behaviour
	{
	public:
		void OnTrigger(EntityID aId) override;

	private:
	};

}