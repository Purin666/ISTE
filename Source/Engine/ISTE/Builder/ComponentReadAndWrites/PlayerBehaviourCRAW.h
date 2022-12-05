#pragma once

#include "ComponentReadAndWrite.h"

namespace ISTE
{
	class PlayerBehaviourCRAW : public ComponentReadAndWrite
	{
	public:
		nlohmann::json Write(void* aComponent, EntityID aEntity);
		void Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson);

	private:

	};

}