#pragma once 
#include "Json/json.hpp"
#include "ComponentReadAndWrite.h"

namespace ISTE
{
	class DecalComponentCRAW : public ComponentReadAndWrite
	{
	public:
		nlohmann::json Write(void* aComponent, EntityID aEntity) override;
		void Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson) override;

	private:

	};
}