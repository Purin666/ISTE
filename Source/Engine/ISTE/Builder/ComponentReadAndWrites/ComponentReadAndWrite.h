#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"
#include "Json/json.hpp"

namespace ISTE
{
	class Scene;

	class ComponentReadAndWrite
	{
	public:
		virtual ~ComponentReadAndWrite() = default;
		virtual nlohmann::json Write(void* aComponent, EntityID aEntity) = 0;
		virtual void Read(void* aComponent, EntityID aEntity, nlohmann::json& aJson) = 0;
		virtual void Assign(Scene&, EntityIndex){};
	};
}