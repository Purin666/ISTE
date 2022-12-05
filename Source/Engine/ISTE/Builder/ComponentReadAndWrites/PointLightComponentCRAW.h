#pragma once

#include "ISTE/Builder/ComponentReadAndWrites/ComponentReadAndWrite.h"

namespace ISTE
{
	class PointLightComponentCRAW : public ComponentReadAndWrite
	{
	public:
		nlohmann::json Write(void* aComponent, EntityID aEntity) override;
		void Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson) override;

	private:
	};
}