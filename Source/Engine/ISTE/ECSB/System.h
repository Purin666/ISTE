#pragma once

#include "ECSDefines.hpp"

#include <set>
#include <map>

#define ECSBSYSTEM_IF_ANY 0x01
#define ECSBSYSTEM_IGNORE_DEACTIVATED 0x02
#define ECSBSYSTEM_ONLY_ACCEPT_ROOT 0x04

typedef uint16_t SYSTEM_FLAG;

namespace ISTE
{
	class System
	{
	public:
		virtual ~System() = default;


	protected:
		friend class SystemManager;
		
		std::map<int, std::set<EntityID>> myEntities;
		std::map <int, bool> myIfAny;
		std::map<int, bool> myIgnoreDeactivated;
		std::map<int, bool> myOnlyAcceptRoot;
		std::map<int, SYSTEM_FLAG> myFlags;
		std::map<int, ComponentMask> myIgnoreIfMasks;
		std::map <int, ComponentMask> mySingnatures;
	};
}